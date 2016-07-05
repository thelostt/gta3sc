#include "flow.hpp"

// TODO dcmd->id & 0x7FFF
// TODO should we get rid of 0-length blocks?

optional<std::string> find_script_name(const Commands& commands, const BlockList& block_list, const ProcEntry& entry_point)
{
    optional<std::string> result;

    depth_first(call_graph, block_list, entry_point, true, [&](const ProcEntry& proc) {
        if(result = find_script_name(commands, block_list, proc.block_id))
        {
            return false;
        }
        return true;
    });

    return result;
}

optional<std::string> find_script_name(const Commands& commands, const BlockList& block_list, BlockId start_block)
{
    optional<std::string> result;

    depth_first(block_list, start_block, true, [&](BlockId block_id) {
        auto& block = block_list.block(block_id);
        for(auto it = block.begin(block_list), end = block.end(block_list); it != end; ++it)
        {
            if(is<DecompiledCommand>(it->data))
            {
                auto& dcmd = get<DecompiledCommand>(it->data);
                if(dcmd.id == commands.script_name().id)
                {
                    Expects(dcmd.args.size() == 1);
                    result = get_immstr(dcmd.args[0]);
                    return false;
                }
            }
        }
        return true;
    });

    return result;
}

BlockList find_basic_blocks(const Commands& commands, const Disassembler& main_segment, const std::vector<Disassembler>& mission_segments)
{
    struct ProcEntry1
    {
        ProcType     type;
        SegReference seg_entry;
    };

    using ForwardEdge = std::pair<SegReference, SegReference>; // jumps from .first to .second

    std::vector<SegReference> leaders;          // **All** possible labels / start of basic blocks.
    std::vector<ProcEntry1> proc_entries1;    // Entry points, including for gosubs
    //std::vector<ForwardEdge>  forward_edges;

    auto segref_find_end = [&](SegType segtype, size_t segindex) -> SegReference
    {
        if(segtype == SegType::Main)
            return SegReference { 0, segtype, segindex, main_segment.get_data().size() };
        else if(segtype == SegType::Mission)
            return SegReference { 0, segtype, segindex, mission_segments[segindex].get_data().size() };
        else
            Unreachable();
    };

    auto segref_by_loffset = [&](const Disassembler& segment_asm, SegType segtype, size_t segindex, uint32_t local_offset) -> SegReference
    {
        if(auto opt_idx = segment_asm.get_dataindex(local_offset))
            return SegReference { 0, segtype, segindex, *opt_idx };
        else
            Expects(false); // TODO
    };

    auto segref_by_label = [&](const Disassembler& segment_asm, SegType segtype, size_t segindex, const ArgVariant2& label_param) -> SegReference
    {
        if(auto opt_imm32 = get_imm32(label_param))
        {
            if(*opt_imm32 >= 0)
                return segref_by_loffset(main_segment, SegType::Main, 0, *opt_imm32);
            else
                return segref_by_loffset(segment_asm, segtype, segindex, -(*opt_imm32));
        }
        else
        {
            Expects(false); // TODO
        }
    };

    auto segref_by_mission = [&](const ArgVariant2& index_param) -> SegReference
    {
        if(auto opt_imm32 = get_imm32(index_param))
        {
            if(*opt_imm32 < mission_segments.size())
                return segref_by_loffset(mission_segments[*opt_imm32], SegType::Mission, *opt_imm32, 0);
            else
                Expects(false); // TODO
        }
        else
        {
            Expects(false); // TODO
        }
    };

    auto find_leaders = [&](const Disassembler& segment_asm, SegType segtype, size_t segindex)
    {
        auto& decompiled = segment_asm.get_data();
        for(size_t i = 0; i < decompiled.size(); ++i)
        {
            if(is<DecompiledLabelDef>(decompiled[i].data))
                continue;

            if(is<DecompiledHex>(decompiled[i].data))
            {
                leaders.emplace_back(SegReference { 0, segtype, segindex, i });
                continue;
            }

            assert(is<DecompiledCommand>(decompiled[i].data));
            auto& dcmd = get<DecompiledCommand>(decompiled[i].data);

            //SegReference segref_this { 0, segtype, segindex, i };


            if(dcmd.id == commands.goto_().id
            || dcmd.id == commands.goto_if_false().id
            || dcmd.id == commands.goto_if_true().id)
            {
                Expects(dcmd.args.size() == 1);
                auto segref_goto = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                auto segref_skip = SegReference{ 0, segtype, segindex, i + 1 };
                leaders.emplace_back(segref_skip);
                leaders.emplace_back(segref_goto);
                //forward_edges.emplace_back(segref_this, segref_goto);
                //forward_edges.emplace_back(segref_this, segref_skip);
            }
            else if(dcmd.id == commands.return_().id
                 || dcmd.id == commands.terminate_this_script().id)
            {
                auto segref_skip = SegReference { 0, segtype, segindex, i + 1 };
                leaders.emplace_back(segref_skip);
            }
            else if(dcmd.id == commands.gosub().id
                 || dcmd.id == commands.gosub_file().id)
            {
                Expects(dcmd.args.size() >= 1);
                auto segref = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                leaders.emplace_back(segref);
                //gosub_targets.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntry1 { ProcType::Gosub, segref });
            }
            else if(dcmd.id == commands.start_new_script().id)
            {
                Expects(dcmd.args.size() >= 1);
                auto segref = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                leaders.emplace_back(segref);
                //script_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntry1{ ProcType::Script, segref });
            }
            else if(dcmd.id == commands.launch_mission().id)
            {
                Expects(dcmd.args.size() == 1);
                auto segref = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                leaders.emplace_back(segref);
                //subscript_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntry1{ ProcType::Subscript, segref });
            }
            else if(dcmd.id == commands.load_and_launch_mission().id)
            {
                Expects(dcmd.args.size() == 1);
                auto segref = segref_by_mission(dcmd.args[0]);
                leaders.emplace_back(segref);
                //mission_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntry1{ ProcType::Mission, segref });
            }
            // TODO more (cutscene thing, etc)
            // TODO make a assert (debug-only) regarding cmd having a label param and we didn't handle it

        }
    };


    // Put main entry point on leaders and proc entries
    {
        auto segref_start = SegReference{ 0, SegType::Main, 0, 0 };
        leaders.emplace_back(segref_start);
        proc_entries1.emplace_back(ProcEntry1{ ProcType::Main, segref_start });
    }

    find_leaders(main_segment, SegType::Main, 0);
    for(size_t i = 0; i < mission_segments.size(); ++i)
        find_leaders(mission_segments[i], SegType::Mission, i);



    // Leaders (and thus Blocks) must be sorted by absolute offset.
    std::sort(leaders.begin(), leaders.end());
    leaders.erase(std::unique(leaders.begin(), leaders.end()), leaders.end());


    //
    std::vector<Block> blocks;
    std::vector<ProcEntry> proc_entries;

    blocks.reserve(leaders.size() + 64); // +64 for dummy nodes
    proc_entries.reserve(proc_entries1.size());

    for(auto it = leaders.begin(); it != leaders.end(); ++it)
    {
        auto it_next = std::next(it);

        SegReference begin = *it;
        SegReference end = it_next != leaders.end()
                        && it_next->segtype == it->segtype
                        && it_next->segindex == it->segindex? *it_next : segref_find_end(it->segtype, it->segindex);
        blocks.emplace_back(Block { begin, end.data_index - begin.data_index });

        if(begin != end)
        {
            // TODO improve this, too many iterations doing a find
            auto proc_it = std::find_if(proc_entries1.begin(), proc_entries1.end(), [&](const ProcEntry1& x) {
                return x.seg_entry == begin;
            });
            if(proc_it != proc_entries1.end())
            {
                proc_entries.emplace_back(ProcEntry { proc_it->type, blocks.size() - 1 });
            }
        }
    }

    //
    BlockList::block_range              main_blocks;
    std::vector<BlockList::block_range> mission_blocks;
    BlockList::find_ranges(blocks, main_blocks, mission_blocks);

    size_t blocks_size = blocks.size();

    return BlockList {
        std::move(blocks),  std::make_pair(0, blocks_size), std::move(main_blocks), std::move(mission_blocks),
        std::move(proc_entries), std::cref(main_segment), std::cref(mission_segments),
    };
}

void find_edges(BlockList& block_list, const Commands& commands)
{
    for(auto block = block_list.begin(); block != block_list.end(); ++block)
    {
        if(block->length == 0)
            continue;

        auto& current_seg = block->block_begin;
        auto branch_it    = std::prev(block->end(block_list));

        if(!is<DecompiledCommand>(branch_it->data))
            continue;

        auto& dcmd = get<DecompiledCommand>(branch_it->data);

        BlockId this_block_id = std::distance(block_list.begin(), block);

        if(dcmd.id == commands.goto_().id
        || dcmd.id == commands.goto_if_false().id
        || dcmd.id == commands.goto_if_true().id)
        {
            int32_t target_label    = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
            BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

            block_list.link_blocks(this_block_id, target_block_id);

            if(dcmd.id != commands.goto_().id)
            {
                block_list.link_blocks(this_block_id, this_block_id + 1);
            }
        }
        else if(dcmd.id == commands.return_().id
             || dcmd.id == commands.terminate_this_script().id)
        {
            // this block does not exit into no other block
        }
        else
        {
            // other non-branchful operations
            block_list.link_blocks(this_block_id, this_block_id + 1);
        }
    }
}

void find_call_edges(BlockList& block_list, const Commands& commands)
{
    for(auto& entry_point : block_list.proc_entries)
    {
        depth_first(block_list, entry_point.block_id, true, [&](BlockId dpfs_id)
        {
            auto& block = block_list.block(dpfs_id);
            auto& current_seg = block.block_begin;

            for(auto it = block.begin(block_list), end = block.end(block_list); it != end; ++it)
            {
                if(!is<DecompiledCommand>(it->data))
                    continue;

                auto& dcmd = get<DecompiledCommand>(it->data);

                if(dcmd.id == commands.gosub().id
                || dcmd.id == commands.gosub_file().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_call(dpfs_id, entry_point, block_list.find_proc_by_entry(target_block_id).value());
                }
                else if(dcmd.id == commands.start_new_script().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_script_spawn(dpfs_id, entry_point, block_list.find_proc_by_entry(target_block_id).value());
                }
                else if(dcmd.id == commands.launch_mission().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_script_spawn(dpfs_id, entry_point, block_list.find_proc_by_entry(target_block_id).value());
                }
                else if(dcmd.id == commands.load_and_launch_mission().id)
                {
                    int32_t target_mission = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_mission(target_mission).value(); // TODO what if fails

                    block_list.link_script_spawn(dpfs_id, entry_point, block_list.find_proc_by_entry(target_block_id).value());
                }
            }

            return true;
        });
    }
}

void BlockList::find_ranges(const std::vector<Block>& blocks, block_range& main_blocks, std::vector<block_range>& mission_blocks)
{
    // TODO what if missions aren't sorted by offset

    SegType  current_segtype  = SegType::Main;
    uint16_t current_segindex = 0;
    BlockId  began_blockid    = 0;

    main_blocks = std::make_pair(0, 0);
    // TODO take care of unfilled spaces in mission_blocks at the end of the function

    for(auto it = blocks.begin(); /* loop until (inclusive) it == end */; ++it)
    {
        SegType it_segtype = (it != blocks.end()? it->block_begin.segtype  : current_segtype);
        size_t it_segindex = (it != blocks.end()? it->block_begin.segindex : current_segindex+1);

        if(current_segtype != it_segtype
        || (current_segtype == SegType::Main && it == blocks.end()))
        {
            BlockId end_block_id = std::distance(blocks.begin(), it);

            if(current_segtype == SegType::Main)
                main_blocks = std::make_pair(began_blockid, end_block_id);
            else if(current_segtype == SegType::Mission)
                Unreachable();
            else
                Unreachable();

            began_blockid = end_block_id;
            current_segtype = it_segtype;
            current_segindex = 0;

            // TODO take care of when going from Main straight to Streamed.
            //

            if(current_segtype == SegType::Mission)
            {
                while(current_segindex != it_segindex)
                {
                    mission_blocks.emplace_back(began_blockid, end_block_id);
                    ++current_segindex;
                }
            }
        }
        else if(current_segindex != it_segindex)
        {
            Expects(current_segtype == SegType::Mission);

            BlockId end_block_id = std::distance(blocks.begin(), it);

            mission_blocks.emplace_back(began_blockid, end_block_id);

            began_blockid = end_block_id;
            while(++current_segindex != it_segindex)
            {
                mission_blocks.emplace_back(began_blockid, end_block_id);
            }
        }

        if(it == blocks.end())
            break;
    }
}

optional<BlockId> BlockList::block_from_label(const SegReference& current_seg, int32_t label_target)
{
    if(label_target < 0 && current_seg.segtype == SegType::Main)
    {
        label_target = -label_target;
    }

    if(label_target >= 0)
    {
        for(BlockId block_id = main_blocks.first; block_id != main_blocks.second; ++block_id)
        {
            auto& block = this->blocks[block_id];

            if(block.length == 0)
                continue;

            size_t x = block.block_begin.data_index;
            if(this->main_segment.get_data()[x].offset == label_target)
                return block_id;
        }
        return nullopt;
    }
    else if(current_seg.segtype == SegType::Mission)
    {
        auto& mission_block = this->mission_blocks[current_seg.segindex];
        auto& mission_segment = this->mission_segments[current_seg.segindex];
        for(BlockId block_id = mission_block.first; block_id != mission_block.second; ++block_id)
        {
            size_t x = this->blocks[block_id].block_begin.data_index;
            if(mission_segment.get_data()[x].offset == -label_target)
                return block_id;
        }
        return nullopt;
    }
    else
    {
        Unreachable();
    }
}

optional<BlockId> BlockList::block_from_mission(int32_t mission_id)
{
    return this->mission_blocks[mission_id].first;
}
