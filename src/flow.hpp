#pragma once
#include "disassembler.hpp"

// http://www.sable.mcgill.ca/~hendren/621/ControlFlowAnalysis_Handouts.pdf

class ScriptFlow
{
public:
    struct Block
    {
        size_t begin;
        size_t end;
    };

private:
    const std::vector<DecompiledData>& decompiled;

    std::vector<uint32_t> local_offsets;
    std::vector<uint32_t> mission_offsets;

    std::vector<std::pair<uint32_t, size_t>> indices; //< Indices by offset

    std::vector<Block> blocks;

public:
    ScriptFlow(const std::vector<DecompiledData>& decompiled)
        : decompiled(decompiled)
    {}

    // blocks are sorted
    const std::vector<Block>& get_blocks() const
    {
        return this->blocks;
    }

public:
    struct less_key
    {
        template<typename T, typename U>
        bool operator()(const T& a, const U& b) const
        {
            return a.first < b.first;
        }
    };

    // step 1
    void find_indices()
    {
        this->indices.reserve(decompiled.size());
        for(size_t i = 0; i < decompiled.size(); ++i)
        {
            this->indices.emplace_back(decompiled[i].offset, i);
        }
        std::sort(indices.begin(), indices.end(), less_key());
    }

    // step 2
    void find_blocks()
    {
        std::vector<size_t> leaders;
        // TODO reserve

        leaders.emplace_back(0);

        for(size_t i = 0; i < decompiled.size(); ++i)
        {
            if(is<DecompiledCommand>(decompiled[i].data))
            {
                auto& dcmd = get<DecompiledCommand>(decompiled[i].data);
                auto offset = decompiled[i].offset;

                switch(dcmd.id & 0x7FFF)
                {
                    case 0x0002: // GOTO
                    case 0x004C: // GOTO_IF_TRUE
                    case 0x004D: // GOTO_IF_FALSE
                    {
                        Expects(dcmd.args.size() >= 1);
                        leaders.emplace_back(i+1);
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x0050: // GOSUB
                    case 0x02DC: // GOSUB_FILE
                    {
                        Expects(dcmd.args.size() >= 1);
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x004F: // START_NEW_SCRIPT
                    case 0x00D7: // LAUNCH_MISSION
                    {
                        Expects(dcmd.args.size() >= 1);
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x0417: // LOAD_AND_LAUNCH_MISSION
                    {
                        Expects(dcmd.args.size() == 1);
                        if(auto opt_mission = get_imm32(dcmd.args[0]))
                            leaders.emplace_back(index_by_mission(*opt_mission));
                        else
                            /* TODO fallback */;
                        break;
                    }
                }
            }
        }

        std::sort(leaders.begin(), leaders.end());
        std::unique(leaders.begin(), leaders.end());

        for(auto it = leaders.begin(); it != leaders.end(); ++it)
        {
            size_t begin_idx = *it;
            size_t end_idx   = it+1 != leaders.end()? *(it+1) : (decompiled.size() - 1);
            this->blocks.emplace_back(Block { begin_idx, end_idx });
        }
    }

    size_t index_by_label(int32_t addressed_from, int32_t label) const
    {
        uint32_t offset = get_absolute_offset(addressed_from, label);

        auto lb = std::lower_bound(this->indices.begin(), this->indices.end(), std::make_pair(offset, -1), less_key());
        if(lb != this->indices.end())
        {
            Expects(lb->first == offset); // TODO fallback
            return lb->second;
        }
        else
        {
            // TODO fallback
            Expects(false);
        }
    }

    size_t index_by_label(int32_t addressed_from, const ArgVariant2& arg_label) const
    {
        if(auto opt_target = get_imm32(arg_label))
            return index_by_label(addressed_from, *opt_target);
        else
            Expects(false); // TODO fallback
    }

    size_t index_by_mission(int32_t mission_id) const
    {
        if(mission_id < mission_offsets.size())
            return mission_offsets[mission_id];
        else
            Expects(false); // TODO fallback
    }



    /*
    bool is_branchful(uint16_t opcode) const
    {
        return opcode == 0x0002 // GOTO
            || opcode == 0x004c // GOTO_IF_TRUE
            || opcode == 0x004d // GOTO_IF_FALSE
            || opcode == 0x004f // START_NEW_SCRIPT
            || opcode == 0x0050 // GOSUB
            || opcode == 0x00d7 // LAUNCH_MISSION
            || opcode == 0x0417 // LOAD_AND_LAUNCH_MISSION
            || opcode == 0x02dc // GOSUB_FILE
           ;
    }
    */

public:
    // !!!!!! copied straight from disassembler.hpp !!!!!!
    void analyze_header(const DecompiledScmHeader& header)
    {
        this->local_offsets = header.mission_offsets;
        this->mission_offsets = header.mission_offsets;
        std::sort(local_offsets.begin(), local_offsets.end());
    }

private:
    // !!!!!! copied straight from disassembler.hpp !!!!!!
    uint32_t get_absolute_offset(uint32_t addressed_from, int32_t offset) const
    {
        if(offset >= 0)
            return offset;

        auto it = std::upper_bound(this->local_offsets.begin(), this->local_offsets.end(), addressed_from);
        if(it != this->local_offsets.begin())
        {
            return *(--it) + (-offset);
        }
        else
        {
            // it's not possible to have a local offset on this location, TODO?
            return -offset;
        }
    }
};


