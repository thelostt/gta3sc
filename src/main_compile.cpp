#include <stdinc.h>
#include "program.hpp"
#include "parser.hpp"
#include "symtable.hpp"
#include "codegen.hpp"
#include "cdimage.hpp"

using RequiredFrom = std::vector<weak_ptr<const Script>>;
using IncluderPair = std::pair<shared_ptr<Script>, IncluderTable>;
using RequiredPair = std::pair<std::string, RequiredFrom>;

namespace
{
    auto read_script(const std::string& filename, ScriptType type,
                     const Script& main, const Script::SubDir& subdir, ProgramContext& program) -> optional<IncluderPair>;

    auto read_scripts(const std::vector<std::string>& filenames, ScriptType type,
                      const Script& main, const Script::SubDir& subdir, ProgramContext& program) -> std::vector<IncluderPair>;

    auto read_extension_scripts(const Script& main, const Script::SubDir& subdir,
                                const IncluderTable& ictable, ProgramContext& program) -> std::vector<IncluderPair>;

    auto resolve_inclusion(shared_ptr<Script> main, const Script::SubDir& subdir,
                           ProgramContext& program) -> std::tuple<IncluderTable, std::vector<shared_ptr<Script>>>;

    void resolve_requires(const std::vector<RequiredPair>& require_info,
                          IncluderTable& ictable, std::vector<shared_ptr<Script>>& scripts,
                          const Script& main, const Script::SubDir& subdir, 
                          ProgramContext& program);

    auto scan_symbols(IncluderTable&&, std::vector<shared_ptr<Script>>& scripts, ProgramContext& program) -> SymTable;

    auto generate_ir(const SymTable&, std::vector<shared_ptr<Script>>& scripts, ProgramContext& program) -> std::vector<CodeGenerator>;

    void optimize_ir(std::vector<CodeGenerator>&, ProgramContext&);

    void generate_scm(std::vector<CodeGenerator>&);

    auto build_headers(std::vector<CodeGenerator>& gens, const SymTable& symbols, const std::vector<std::string>& models,
                       const shared_ptr<const Script> main, std::vector<shared_ptr<Script>>& scripts,
                       ProgramContext& program) -> MultiFileHeaderList;

    void compute_offsets(std::vector<CodeGenerator>& gens, const MultiFileHeaderList& multi_headers,
                         std::vector<shared_ptr<Script>>& scripts, ProgramContext& program);

    template<typename Writeable1, typename Writeable2> static
    void generate_output(const std::vector<CodeGenerator>& gens,
                         const MultiFileHeaderList& multi_headers,
                         Writeable1& main_scm, Writeable2& script_img, bool has_script_img,
                         ProgramContext& program);

    void check_expect_vars(const Script& main, const SymTable&, ProgramContext&);
}

int compile(fs::path input, fs::path output, ProgramContext& program)
{
    if(output.empty())
    {
        output = fs::path(input).replace_extension([&] {
            if(program.opt.emit_ir2)
                return ".ir2";
            else if(program.opt.output_cleo)
                return program.opt.mission_script? ".cm" : ".cs";
            else
                return ".scm";
        }());
    }

    try
    {
        IncluderTable ictable;
        std::vector<shared_ptr<Script>> scripts;

        const auto main_type = [&] {
            if(program.opt.output_cleo)
                return program.opt.mission_script? ScriptType::CustomMission : ScriptType::CustomScript;
            else
                return program.opt.mission_script? ScriptType::Mission : ScriptType::Main;
        }();

        const auto use_script_img = (program.opt.streamed_scripts && !program.opt.headerless);

        shared_ptr<Script> main = Script::create(input, main_type, program);

        if(!main)
        {
            assert(program.has_error());
            throw ProgramFailure();
        }

        auto subdir = main->scan_subdir();

        std::tie(ictable, scripts) = resolve_inclusion(main, subdir, program);

        if(program.has_error())
            throw ProgramFailure();

        SymTable symbols = scan_symbols(std::move(ictable), scripts, program);
        symbols.check_scope_collisions(program);
        symbols.check_constant_collisions(program);

        if(program.has_error())
            throw ProgramFailure();

        std::for_each(scripts.begin(), scripts.end(), [&](const auto& script) {
            script->annotate_tree(symbols, program);
        });

        if(program.has_error())
            throw ProgramFailure();

        std::for_each(scripts.begin(), scripts.end(), [&](const auto& script) {
            script->compute_scope_outputs(symbols, program);
            script->fix_call_scope_variables(program);
        });

        if(program.has_error())
            throw ProgramFailure();

        check_expect_vars(*main, symbols, program);

        Script::handle_special_commands(scripts, symbols, program);

        if(program.has_error())
            throw ProgramFailure();

        auto models = Script::compute_used_objects(scripts);
        if(program.opt.output_cleo)
        {
            for(auto& model : models)
                program.error(nocontext, "use of non-default model {} in custom script", model);
        }

        auto gens = generate_ir(symbols, scripts, program);

        if(program.has_error())
            throw ProgramFailure();

        if(program.opt.fsyntax_only)
            return EXIT_SUCCESS;

        optimize_ir(gens, program);

        auto multi_headers = build_headers(gens, symbols, models, main, scripts, program);

        compute_offsets(gens, multi_headers, scripts, program);
        
        generate_scm(gens);

        if(program.has_error())
            throw ProgramFailure();

        if(program.opt.emit_ir2)
        {
            FILE *outstream = 0;
            std::vector<uint8_t> main_scm;
            std::vector<uint8_t> script_img;

            auto guard = make_scope_guard([&] {
                if(outstream && outstream != stdout) fclose(outstream);
            });

            outstream = (output != "-"? u8fopen(output, "wb") : stdout);
            if(outstream == nullptr)
                program.fatal_error(nocontext, "failed to open output for writing");

            bool is_first_line = true;
            auto print_ir2_line = [&](const std::string& line)
            {
                if(is_first_line)
                {
                    is_first_line = false;
                    fprintf(outstream, "%s", line.c_str());
                }
                else
                {
                    fprintf(outstream, "\n%s", line.c_str());
                }
            };

            generate_output(gens, multi_headers, main_scm, script_img, use_script_img, program);

            auto status = decompile(main_scm.data(), main_scm.size(),
                                    script_img.data(), script_img.size(), program,
                                    Options::Lang::IR2, print_ir2_line);
            if(!status)
                throw ProgramFailure();
        }
        else
        {
            FILE *main_scm = 0, *script_img = 0;

            auto guard = make_scope_guard([&] {
                if(main_scm) fclose(main_scm);
                if(script_img) fclose(script_img);
            });

            main_scm = u8fopen(output, "wb");
            if(main_scm == nullptr)
                program.fatal_error(nocontext, "failed to open output for writing");

            if(use_script_img)
            {
                script_img = u8fopen(fs::path(output).replace_filename("script.img"), "wb");
                if(!script_img)
                    program.fatal_error(nocontext, "failed to open script.img for writing");
            }

            generate_output(gens, multi_headers, main_scm, script_img, use_script_img, program);
        }
        
        if(program.has_error())
            throw ProgramFailure();

        return EXIT_SUCCESS;
    }
    catch(const ProgramFailure&)
    {
        fprintf(stderr, "gta3sc: compilation failed\n");
        return EXIT_FAILURE;
    }
}

namespace
{

auto read_script(const std::string& filename, ScriptType type,
                 const Script& main, const Script::SubDir& subdir, ProgramContext& program) -> optional<IncluderPair>
{
    if(auto script = main.from_subdir(filename, subdir, type, program))
    {
        return std::make_pair( script, IncluderTable::from_script(*script, program) );
    }
    return nullopt;
}

auto read_scripts(const std::vector<std::string>& filenames, ScriptType type,
                  const Script& main, const Script::SubDir& subdir, ProgramContext& program) -> std::vector<IncluderPair>
{
    std::vector<IncluderPair> output;
    output.reserve(filenames.size());

    std::for_each(filenames.begin(), filenames.end(), [&](const auto& filename) {
        if(auto ic_pair = read_script(filename, type, main, subdir, program))
            output.emplace_back(std::move(*ic_pair));
    });

    return output;
}

auto read_extension_scripts(const Script& main, const Script::SubDir& subdir,
                            const IncluderTable& ictable, ProgramContext& program) -> std::vector<IncluderPair>
{
    std::vector<IncluderPair> output;

    insensitive_set<std::string> readen;

    std::deque<std::string> to_read;
    std::copy(ictable.extfiles.begin(), ictable.extfiles.end(), std::back_inserter(to_read));

    while(!to_read.empty())
    {
        auto topname = std::move(to_read.front());
        to_read.pop_front();

        if(!readen.count(topname))
        {
            if(auto ic_pair = read_script(topname, ScriptType::MainExtension, main, subdir, program))
            {
                output.emplace_back(std::move(*ic_pair));
                readen.emplace(std::move(topname));

                auto& script_ictable = output.back().second;
                std::copy(script_ictable.extfiles.begin(), script_ictable.extfiles.end(), std::back_inserter(to_read));
            }
        }
    }

    return output;
}

auto resolve_inclusion(shared_ptr<Script> main, const Script::SubDir& subdir,
                       ProgramContext& program) -> std::tuple<IncluderTable, std::vector<shared_ptr<Script>>>
{
    std::vector<shared_ptr<Script>> scripts;
    std::vector<RequiredPair> require_info;

    auto identify_requires = [&](const shared_ptr<const Script>& script, const IncluderTable& ictable)
    {
        for(auto& rqname : ictable.required)
        {
            auto it = std::find_if(require_info.begin(), require_info.end(), [&](const auto& a) { return iequal_to()(a.first, rqname); });
            if(it == require_info.end())
            {
                it = require_info.emplace(require_info.end(), RequiredPair(rqname, {}));
            }
            it->second.emplace_back(script);
        }
    };

    auto ictable = IncluderTable::from_script(*main, program);

    identify_requires(main, ictable);
    scripts.emplace_back(main);

    for(auto& ic_pair : read_extension_scripts(*main, subdir, ictable, program))
    {
        identify_requires(ic_pair.first, ic_pair.second);
        ictable.merge(std::move(ic_pair.second), program);
        scripts.emplace_back(ic_pair.first);
    }

    auto sub_scripts = read_scripts(ictable.subscript, ScriptType::Subscript, *main, subdir, program);
    auto mission_scripts = read_scripts(ictable.mission, ScriptType::Mission, *main, subdir, program);
    auto streamed_scripts = read_scripts(ictable.streamed, ScriptType::StreamedScript, *main, subdir, program);

    for(auto& ic_pair : sub_scripts)
    {
        identify_requires(ic_pair.first, ic_pair.second);
        ictable.merge(std::move(ic_pair.second), program);
        scripts.emplace_back(ic_pair.first);
    }

    {
        size_t i = 0;
        for(auto& ic_pair : mission_scripts)
        {
            identify_requires(ic_pair.first, ic_pair.second);
            ictable.merge(std::move(ic_pair.second), program);
            scripts.emplace_back(ic_pair.first);
            scripts.back()->mission_id = static_cast<uint16_t>(i++);
        }
    }

    {
        size_t i = 0;
        for(auto& ic_pair : streamed_scripts)
        {
            identify_requires(ic_pair.first, ic_pair.second);
            ictable.merge(std::move(ic_pair.second), program);
            scripts.emplace_back(ic_pair.first);
            scripts.back()->streamed_id = static_cast<uint16_t>(i++);
        }
    }

    resolve_requires(require_info, ictable, scripts, *main, subdir, program);

    return {std::move(ictable), std::move(scripts)};
}

void resolve_requires(const std::vector<RequiredPair>& require_info,
                      IncluderTable& ictable, std::vector<shared_ptr<Script>>& scripts,
                      const Script& main, const Script::SubDir& subdir, ProgramContext& program)
{
    auto req_scripts = insensitive_map<std::string, IncluderPair>();
    std::for_each(require_info.begin(), require_info.end(), [&](const auto& vpair) {
        if(auto opt = read_script(vpair.first, ScriptType::Required, main, subdir, program))
        {
            req_scripts.emplace(vpair.first, std::move(*opt));
        }
    });

    // insert the required scripts into the `scripts` list.
    for(auto it = require_info.rbegin(); it != require_info.rend(); ++it)
    {
        auto req_pair = req_scripts.find(it->first);
        if(req_pair != req_scripts.end())
        {
            auto& required_script = req_pair->second.first;
            auto& required_from = it->second;

            const auto insert_after = [&] {
                if(required_from.size() == 1)
                    return required_from.front().lock();

                if(std::none_of(required_from.begin(), required_from.end(), [&](const auto& from) {
                    return from.lock()->on_the_same_space_as(main);
                }))
                {
                    program.error(required_script, "script was required from multiple mission/streamed scripts but never from the main block");
                }

                return main.shared_from_this();
            }();

            auto it_after = std::find(scripts.begin(), scripts.end(), insert_after);
            assert(it_after != scripts.end());

            (*it_after)->add_children(required_script);
            ictable.merge(std::move(req_pair->second.second), program);
            scripts.insert(std::next(it_after), required_script);
        }
    }
}

auto scan_symbols(IncluderTable&& ictable, std::vector<shared_ptr<Script>>& scripts, ProgramContext& program) -> SymTable
{
    SymTable symbols { std::move(ictable) };
    symbols.apply_offset_to_vars(2);

    std::vector<SymTable> vec_symbols;
    vec_symbols.reserve(scripts.size());

    std::transform(scripts.begin(), scripts.end(), std::back_inserter(vec_symbols), [&](const auto& script) {
        return SymTable::from_script(*script, program);
    });

    for(auto& other_table : vec_symbols)
    {
        symbols.merge(std::move(other_table), program);
    }

    symbols.build_script_table(scripts);
    return symbols;
}

auto build_headers(std::vector<CodeGenerator>& gens, const SymTable& symbols, const std::vector<std::string>& models,
                   const shared_ptr<const Script> main, std::vector<shared_ptr<Script>>& scripts, ProgramContext& program) -> MultiFileHeaderList
{
    MultiFileHeaderList multi_headers;

    std::vector<CodeGenerator*> main_gens;
    std::vector<std::vector<CodeGenerator*>> mission_gens;
    std::vector<std::vector<CodeGenerator*>> stream_gens;

    main_gens.reserve(scripts.size());
    mission_gens.reserve(scripts.size());
    stream_gens.reserve(scripts.size());

    for(auto& gen : gens)
    {
        auto root_script = gen.script->root_script();
        if(root_script->on_the_same_space_as(*main))
        {
            main_gens.emplace_back(&gen);
        }
        else if(root_script->type == ScriptType::Mission)
        {
            auto id = root_script->mission_id.value();
            if(id <= mission_gens.size()) mission_gens.resize(id+1);
            mission_gens[id].emplace_back(&gen);
        }
        else if(root_script->type == ScriptType::StreamedScript)
        {
            auto id = root_script->streamed_id.value();
            if(id <= stream_gens.size()) stream_gens.resize(id+1);
            stream_gens[id].emplace_back(&gen);
        }
        else
        {
            Unreachable();
        }
    }

    if(!program.opt.headerless)
    {
        CompiledScmHeader hscm(program.opt.get_header<CompiledScmHeader::Version>(), symbols.size_global_vars(), models, scripts);
        multi_headers.add_header(main, std::move(hscm));
    }

    if(program.opt.oatc)
    {
        auto& main_oatc = multi_headers.add_header(main, CustomHeaderOATC(main_gens, program));
        for(auto& pgen : main_gens) pgen->set_oatc(main_oatc);

        for(auto& pgenvec : mission_gens)
        {
            auto& oatc = multi_headers.add_header(pgenvec[0]->script->root_script(), CustomHeaderOATC(pgenvec, program));
            for(auto& pgen : pgenvec) pgen->set_oatc(oatc);
        }

        for(auto& pgenvec : stream_gens)
        {
            auto& oatc = multi_headers.add_header(pgenvec[0]->script->root_script(), CustomHeaderOATC(pgenvec, program));
            for(auto& pgen : pgenvec) pgen->set_oatc(oatc);
        }
    }

    return multi_headers;
}

void compute_offsets(std::vector<CodeGenerator>& gens, const MultiFileHeaderList& multi_headers,
                     std::vector<shared_ptr<Script>>& scripts, ProgramContext& program)
{
    assert(gens.size() == scripts.size());

    for_loop(size_t(0), gens.size(), [&](size_t i) {
        scripts[i]->code_size = gens[i].compute_labels();
    });

    Script::compute_script_offsets(scripts, multi_headers);
}

auto generate_ir(const SymTable& symbols, std::vector<shared_ptr<Script>>& scripts, ProgramContext& program) -> std::vector<CodeGenerator>
{
    std::vector<CodeGenerator> gens;
    gens.reserve(scripts.size());

    std::transform(scripts.begin(), scripts.end(), std::back_inserter(gens), [&](const auto& script) {
        return CodeGenerator { CompilerContext::compile(script, symbols, program), program };
    });

    return gens;
}

void optimize_ir(std::vector<CodeGenerator>& gens, ProgramContext& program)
{
    if(!program.opt.optimize_with_peepholer)
        return;

    std::for_each(gens.begin(), gens.end(), [&](auto& gen) {
        gen.peepholer(program);
    });
}

void generate_scm(std::vector<CodeGenerator>& gens)
{
    std::for_each(gens.begin(), gens.end(), [&](auto& gen) {
        gen.generate();
    });
}

template<typename Writeable1, typename Writeable2>
void generate_output(const std::vector<CodeGenerator>& gens,
                     const MultiFileHeaderList& multi_headers,
                     Writeable1& main_scm, Writeable2& script_img, bool has_script_img,
                     ProgramContext& program)
{
    auto write_headers = [&](auto& output_file, size_t offset, const shared_ptr<const Script>& script) -> size_t
    {
        size_t total_size = 0;
        if(auto opt = multi_headers.script_headers(script))
        {
            for(auto& header : *opt)
            {
                CodeGeneratorData hgen(script, total_size, header, program);
                hgen.generate();
                write_file(output_file, offset, hgen.buffer(), hgen.buffer_size());
                total_size += hgen.buffer_size();
                offset += hgen.buffer_size();
            }
        }
        return total_size;
    };

    std::vector<std::pair<std::string, const CodeGenerator*>> into_script_img;

    assert(gens[0].script->is_main_script());
    auto scmheader = multi_headers.find_header<CompiledScmHeader>(gens[0].script);

    size_t multifile_size = std::accumulate(gens.begin(), gens.end(), size_t(0), [&](size_t size, const auto& gen) {
        if(gen.script->is_root_script() && gen.script->type != ScriptType::StreamedScript)
            return size + gen.script->full_size();
        return size;
    });

    if(!allocate_file_space(main_scm, multifile_size))
        program.fatal_error(nocontext, "failed to allocate disk space for the main file");

    for(const auto& gen : gens)
    {
        if(!gen.script->is_child_of(ScriptType::StreamedScript))
        {
            write_headers(main_scm, gen.script->base.value(), gen.script);
            write_file(main_scm, gen.script->code_offset.value(), gen.buffer(), gen.buffer_size());
        }
        else
        {
            if(gen.script->type != ScriptType::Required)
                into_script_img.emplace_back(gen.script->path.stem().u8string(), &gen);
        }
    }

    std::sort(into_script_img.begin(), into_script_img.end(), [](const auto& lhs, const auto& rhs) {
        return iless()(lhs.first, rhs.first);
    });

    if(has_script_img)
    {
        struct alignas(4) AAAScript
        {
            uint32_t size_global_space;
            uint8_t unknown0 = 62;
            uint8_t unknown1  = 2;
            uint16_t unknown2 = 0;
        };

        auto round_2kb = [](size_t size) -> size_t
        {
            return (size + 2048 - 1) & ~(2048 - 1);
        };

        AAAScript aaa_scm;
        aaa_scm.size_global_space = scmheader->size_global_vars_space - 8;

        CdHeader cd_header { {'V','E','R','2'}, static_cast<uint32_t>(1 + into_script_img.size()) };
        std::vector<CdEntry> directory;
        directory.reserve(1 + into_script_img.size());

        std::string temp_filename;
        size_t files_offset = round_2kb(sizeof(CdHeader) + ((1 + into_script_img.size()) * sizeof(CdEntry)));
        size_t end_offset = 0;

        auto add_entry = [&](const char* filename, size_t size)
        {
            CdEntry next_entry;

            if(directory.empty())
                next_entry.offset = round_2kb(files_offset) / 2048;
            else
                next_entry.offset = directory.back().offset + directory.back().streaming_size;

            next_entry.streaming_size = static_cast<uint16_t>(round_2kb(size) / 2048);

            strncpy(next_entry.filename, filename, 23);
            next_entry.filename[23] = 0;

            directory.emplace_back(next_entry);
        };

        add_entry("aaa.scm", 8);
        for(auto& into : into_script_img)
        {
            auto& script = into.second->script;
            temp_filename = script->path.stem().u8string();
            temp_filename += ".scm";
            add_entry(temp_filename.c_str(), script->full_size());
        }

        if(directory.empty())
            end_offset = files_offset;
        else
            end_offset = (directory.back().offset + directory.back().streaming_size) * 2048;

        if(!allocate_file_space(script_img, end_offset))
            program.fatal_error(nocontext, "failed to allocate disk space for script.img");

        write_file(script_img, 0, &cd_header, sizeof(cd_header));
        write_file(script_img, sizeof(cd_header), directory.data(), sizeof(CdEntry) * directory.size());

        // aaa.scm
        write_file(script_img, directory[0].offset * 2048, &aaa_scm, sizeof(aaa_scm));

        for(size_t i = 0; i < into_script_img.size(); ++i)
        {
            const CodeGenerator& gen = *into_script_img[i].second;

            size_t offset = directory[1+i].offset * 2048;
            offset += write_headers(script_img, offset, gen.script);

            write_file(script_img, offset, gen.buffer(), gen.buffer_size());
            offset += gen.buffer_size();

            for(auto& weakp : gen.script->children_scripts)
            {
                auto required_script = weakp.lock();
                auto& required_gen = *std::find_if(gens.begin(), gens.end(), [&](const auto& g) { return g.script == required_script; });
                write_file(script_img, offset, required_gen.buffer(), required_gen.buffer_size());
                offset += required_gen.buffer_size();
            }
        }

        assert(file_tell(script_img) <= end_offset);
    }

    assert(file_tell(main_scm) == multifile_size);
}

void check_expect_vars(const Script& main, const SymTable& symbols, ProgramContext& program)
{
    if(!program.opt.warn_expect_var || main.type != ScriptType::Main)
        return;

    for(auto& expect : program.opt.expect_vars)
    {
        string_view var_name;
        shared_ptr<const Var> var;

        if(expect.first.size() == 0)
            continue;

        for(auto& name : expect.first)
        {
            if(auto opt_var = symbols.find_var(name, nullptr))
            {
                var_name = name;
                var = *opt_var;
                break;
            }
        }

        if(!var)
        {
            program.warning(nocontext, "expected variable {} to exist, but it does not", expect.first[0]);
            continue;
        }

        if(var->index != expect.second)
        {
            program.warning(nocontext, "expected variable {} to have index {} but it has index {}", 
                                                                var_name.to_string(), expect.second, var->index);
            continue;
        }
    }
}

}
