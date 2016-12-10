#include <stdinc.h>
#include "program.hpp"
#include "disassembler.hpp"
#include "decompiler_ir2.hpp"

int decompile(fs::path input, fs::path output, ProgramContext& program)
{
    if(output.empty())
    {
        output = input;
        output.replace_extension(program.opt.emit_ir2? ".ir2" : ".sc");
    }

    try
    {
        const Commands& commands = program.commands;

        FILE* outstream;

        auto lang = (program.opt.emit_ir2? Options::Lang::IR2 : Options::Lang::GTA3Script);

        auto guard = make_scope_guard([&] {
            if(outstream != stdout) fclose(outstream);
        });

        if(lang == Options::Lang::GTA3Script)
            program.fatal_error(nocontext, "GTA3script output is disabled, please use -emit-ir2 for IR2 output");

        outstream = (output != "-"? u8fopen(output, "wb") : stdout);
        if(!outstream)
            program.fatal_error(nocontext, "could not open file '{}' for writing", output.generic_u8string());

        auto opt_bytecode = read_file_binary(input);
        if(!opt_bytecode)
            program.fatal_error(nocontext, "file '{}' does not exist", input.generic_u8string());

        std::vector<uint8_t> script_img;
        if(program.opt.streamed_scripts)
        {
            auto img_path = fs::path(input).replace_filename("script.img");
            if(auto opt = read_file_binary(img_path))
                script_img = std::move(*opt);
            else
                program.fatal_error(nocontext, "file '{}' does not exist", img_path.generic_u8string());
        }

        auto println = [&](const std::string& line) { fprintf(outstream, "%s\n", line.c_str()); }; 
        if(!decompile(opt_bytecode->data(), opt_bytecode->size(), script_img.data(), script_img.size(), program, lang, println))
            throw ProgramFailure();

        return 0;
    }
    catch(const ProgramFailure&)
    {
        fprintf(stderr, "gta3sc: decompilation failed\n");
        return EXIT_FAILURE;
    }
}

bool decompile(const void* bytecode, size_t bytecode_size,
               const void* script_img, size_t script_img_size,
               ProgramContext& program, Options::Lang lang,
               std::function<void(const std::string&)> callback)
{
    Expects(!program.opt.streamed_scripts || program.opt.headerless || script_img != nullptr);

    try
    {
        optional<DecompiledScmHeader> opt_header;
        size_t ignore_stream_id = -1;

        auto scan_type = program.opt.linear_sweep? Disassembler::Type::LinearSweep :
                                                   Disassembler::Type::RecursiveTraversal;

        if(!program.opt.headerless)
        {
            opt_header = DecompiledScmHeader::from_bytecode(bytecode, bytecode_size,
                                                            program.opt.get_header<DecompiledScmHeader::Version>());
            if(!opt_header)
                program.fatal_error(nocontext, "corrupted scm header");

            DecompiledScmHeader& header = *opt_header;

            auto it = std::find_if(header.streamed_scripts.begin(), header.streamed_scripts.end(), [](const auto& pair) {
                return iequal_to()(pair.name, "AAA");
            });
            if(it != header.streamed_scripts.end())
                ignore_stream_id = (it - header.streamed_scripts.begin());
        }

         
        BinaryFetcher main_segment{ bytecode, std::min<uint32_t>(bytecode_size, opt_header? opt_header->main_size : bytecode_size) };
        std::vector<BinaryFetcher> mission_segments;
        std::vector<BinaryFetcher> stream_segments;

        if(!program.opt.headerless)
        {
            DecompiledScmHeader& header = *opt_header;

            mission_segments = mission_scripts_fetcher(bytecode, bytecode_size, header, program);
            if(program.opt.streamed_scripts)
            {
                stream_segments = streamed_scripts_fetcher(script_img, script_img_size, header, program);
            }
        }

        // TODO add more has error in here?
        if(program.has_error())
            throw ProgramFailure();

        Disassembler main_segment_asm(program, main_segment, scan_type);
        std::vector<Disassembler> mission_segments_asm;
        std::vector<Disassembler> stream_segments_asm;

        if(!program.opt.headerless)
        {
            DecompiledScmHeader& header = *opt_header;

            mission_segments_asm.reserve(header.mission_offsets.size());
            stream_segments_asm.reserve(header.streamed_scripts.size());

            // this loop cannot be thread safely unfolded because of main_segment_asm being
            // mutated on all the units.
            for(auto& mission_bytecode : mission_segments)
            {
                mission_segments_asm.emplace_back(program, mission_bytecode, main_segment_asm, scan_type);
                mission_segments_asm.back().run_analyzer();
            }

            // this loop cannot be thread safely unfolded because of stream_segment_asm being
            // mutated on all the units.
            for(size_t i = 0; i < stream_segments.size(); ++i)
            {
                if(i != ignore_stream_id)
                {
                    auto& stream_bytecode = stream_segments[i];
                    stream_segments_asm.emplace_back(program, stream_bytecode, main_segment_asm, scan_type);
                    stream_segments_asm.back().run_analyzer();
                }
            }
        }

        if(true)
        {
            // run main segment analyzer after the missions and streams analyzer
            main_segment_asm.run_analyzer(opt_header? opt_header->code_offset : 0);
            main_segment_asm.disassembly(opt_header? opt_header->code_offset : 0);
        }

        for(auto& mission_asm : mission_segments_asm)
            mission_asm.disassembly();

        for(size_t i = 0; i < stream_segments.size(); ++i)
        {
            if(i != ignore_stream_id)
            {
                auto& stream_asm = stream_segments_asm[i];
                stream_asm.disassembly();
            }
        }

        if(lang == Options::Lang::IR2)
        {
            if(!program.opt.headerless)
            {
                std::string temp_string;
                for(size_t i = 0; i < opt_header->models.size(); ++i)
                {
                    temp_string = opt_header->models[i];
                    std::transform(temp_string.begin(), temp_string.end(), temp_string.begin(), ::toupper); // TODO FIXME toupper is bad
                    callback(fmt::format("#DEFINE_MODEL {} -{}", temp_string, i+1));
                }
            }

            if(!program.opt.headerless)
            {
                std::string temp_string;
                for(size_t i = 0; i < opt_header->streamed_scripts.size(); ++i)
                {
                    temp_string = opt_header->streamed_scripts[i].name;
                    std::transform(temp_string.begin(), temp_string.end(), temp_string.begin(), ::toupper); // TODO FIXME toupper is bad
                    callback(fmt::format("#DEFINE_STREAM {} {}", temp_string, i));
                }
            }

            auto main_ir2 = DecompilerIR2(program.commands, main_segment_asm.get_data(), 0, main_segment.size, "MAIN", true);
            main_ir2.decompile(callback);

            for(size_t i = 0; i < mission_segments_asm.size(); ++i)
            {
                auto& mission_asm = mission_segments_asm[i];
                auto script_name = fmt::format("MISSION_{}", i);
                callback(fmt::format("#MISSION_BLOCK_START {}", (int)(i)));
                DecompilerIR2(program.commands, mission_asm.get_data(), 0, mission_segments[i].size, std::move(script_name), false, main_ir2).decompile(callback);
                callback("#MISSION_BLOCK_END");
            }

            for(size_t i = 0; i < stream_segments_asm.size(); ++i)
            {
                if(i != ignore_stream_id)
                {
                    auto& stream_asm = stream_segments_asm[i];
                    auto script_name = fmt::format("STREAM_{}", i);
                    callback(fmt::format("#STREAMED_BLOCK_START {}", (int)(i)));
                    DecompilerIR2(program.commands, stream_asm.get_data(), 0, stream_segments[i].size, std::move(script_name), false, main_ir2).decompile(callback);
                    callback("#STREAMED_BLOCK_END");
                }
            }
        }

        if(program.has_error())
            throw ProgramFailure();

        return true;
    }
    catch(const ProgramFailure&)
    {
        return false;
    }
}
