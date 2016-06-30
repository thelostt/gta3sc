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

        bool operator<(const Block& rhs) const
        {
            return this->begin < rhs.begin;
        }
    };

    struct CfgNode
    {
        std::vector<size_t> edges_in;  //< Blocks which branches to [the start of] this block.
        std::vector<size_t> edges_out; //< Blocks which this block branches to [at the end].
    };


private:
    const std::vector<DecompiledData>& decompiled;

    std::vector<uint32_t> local_offsets;
    std::vector<uint32_t> mission_offsets;

    std::vector<std::pair<uint32_t, size_t>> indices; //< Indices by offset

    std::vector<Block> blocks;

    std::vector<CfgNode> cfg_nodes;
    // TODO should we move CfgNode content into Block?

public:
    ScriptFlow(const std::vector<DecompiledData>& decompiled)
        : decompiled(decompiled)
    {}

    // blocks are sorted
    const std::vector<Block>& get_blocks() const
    {
        return this->blocks;
    }

    // cfg_nodes have the same size as blocks and the [index] talks about the same data
    const std::vector<CfgNode>& get_cfg_nodes() const
    {
        return this->cfg_nodes;
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
                        assert(index_by_label(offset, dcmd.args[0]) < decompiled.size());
                        assert(i+1 < decompiled.size());
                        leaders.emplace_back(i+1);
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x0050: // GOSUB
                    case 0x02DC: // GOSUB_FILE
                    {
                        Expects(dcmd.args.size() >= 1);
                        assert(index_by_label(offset, dcmd.args[0]) < decompiled.size());
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x004F: // START_NEW_SCRIPT
                    case 0x00D7: // LAUNCH_MISSION
                    {
                        Expects(dcmd.args.size() >= 1);
                        assert(index_by_label(offset, dcmd.args[0]) < decompiled.size());
                        leaders.emplace_back(index_by_label(offset, dcmd.args[0]));
                        break;
                    }

                    case 0x0417: // LOAD_AND_LAUNCH_MISSION
                    {
                        Expects(dcmd.args.size() == 1);
                        if(auto opt_mission = get_imm32(dcmd.args[0]))
                        {
                            assert(index_by_mission(*opt_mission) < decompiled.size());
                            leaders.emplace_back(index_by_mission(*opt_mission));
                        }
                        else
                            /* TODO fallback */;
                        break;
                    }
                }
            }
        }

        std::sort(leaders.begin(), leaders.end());
        leaders.erase(std::unique(leaders.begin(), leaders.end()), leaders.end());

        for(auto it = leaders.begin(); it != leaders.end(); ++it)
        {
            size_t begin_idx = *it;
            size_t end_idx   = it+1 != leaders.end()? *(it+1) : (decompiled.size() - 1);
            this->blocks.emplace_back(Block { begin_idx, end_idx });
        }
        
        assert(std::is_sorted(this->blocks.begin(), this->blocks.end()));
    }

    // step 3
    void find_edges()
    {
        this->cfg_nodes.resize(this->blocks.size());

        for(size_t block_id = 0; block_id < this->blocks.size(); ++block_id)
        {
            size_t i_ = (this->blocks[block_id].end - 1); // XXX rename var
            if(is<DecompiledCommand>(decompiled[i_].data))
            {
                auto& dcmd = get<DecompiledCommand>(decompiled[i_].data);
                auto offset = decompiled[i_].offset;

                switch(dcmd.id & 0x7FFF)
                {
                    case 0x0002: // GOTO
                    {
                        Expects(dcmd.args.size() >= 1);
                        size_t target_block = blockid_by_label(offset, dcmd.args[0]);
                        this->cfg_nodes[block_id].edges_out.emplace_back(target_block);
                        this->cfg_nodes[target_block].edges_in.emplace_back(block_id);
                        break;
                    }
                    
                    case 0x004C: // GOTO_IF_TRUE
                    case 0x004D: // GOTO_IF_FALSE
                    {
                        Expects(dcmd.args.size() >= 1);
                        size_t branch_block = blockid_by_label(offset, dcmd.args[0]);
                        size_t next_block   = block_id + 1;
                        // TODO think about validity of next_block (^) (i.e. it does not goes out of this->blocks bound)
                        this->cfg_nodes[block_id].edges_out.emplace_back(branch_block);
                        this->cfg_nodes[block_id].edges_out.emplace_back(next_block);
                        this->cfg_nodes[branch_block].edges_in.emplace_back(block_id);
                        this->cfg_nodes[next_block].edges_in.emplace_back(block_id);
                        break;
                    }

                    default: // not a branch opcode
                    {
                        size_t next_block = block_id + 1;
                        // TODO think about validity of next_block (^) (i.e. it does not goes out of this->blocks bound)
                        this->cfg_nodes[block_id].edges_out.emplace_back(next_block);
                        this->cfg_nodes[next_block].edges_in.emplace_back(block_id);
                        break;
                    }
                }
            }
        }
    }

    //////////

    size_t index_by_absolute_offset(uint32_t offset) const
    {
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

    size_t index_by_label(int32_t addressed_from, int32_t label) const
    {
        uint32_t offset = get_absolute_offset(addressed_from, label);
        return index_by_absolute_offset(offset);
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
            return index_by_absolute_offset(mission_offsets[mission_id]);
        else
            Expects(false); // TODO fallback
    }

    //////////

    size_t blockid_by_index(size_t index) const
    {
        auto it = std::lower_bound(this->blocks.begin(), this->blocks.end(), Block { index, SIZE_MAX });
        if(it != this->blocks.end() && it->begin == index)
            return (it - this->blocks.begin());
        else
            Expects(false); // TODO fallback? maybe this really should be unexpected / unreachable?
    }

    size_t blockid_by_label(int32_t addressed_from, const ArgVariant2& arg_label) const
    {
        size_t index = index_by_label(addressed_from, arg_label);
        return blockid_by_index(index);
    }

    //////////



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


