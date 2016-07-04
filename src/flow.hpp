#pragma once
#include "disassembler.hpp"

// TODO dcmd->id & 0x7FFF

using BlockId = size_t;

enum class ProcType : uint8_t
{
    None        = 0x00, //< Should only be used in (a & b) != ProcType::None expressions
    Main        = 0x01, //< Main Entry Point
    Gosub       = 0x02, //< Called with GOSUB or GOSUB_FILE
    Script      = 0x04, //< Spawned with START_NEW_SCRIPT
    Subscript   = 0x08, //< Spawned with LAUNCH_MISSION
    Mission     = 0x10, //< Spawned with LOAD_AND_LAUNCH_MISSION
};

inline ProcType operator|(ProcType a, ProcType b)
{
    return static_cast<ProcType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline ProcType operator&(ProcType a, ProcType b)
{
    return static_cast<ProcType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}



// TODO rename to ProcEntry
struct ProcEntries
{
    ProcType type;
    BlockId  block_id;

    std::vector<BlockId> calls_into;
    std::vector<BlockId> called_from;

    std::vector<BlockId> spawns_script;
    std::vector<BlockId> spawned_from;

    explicit ProcEntries(ProcType type, BlockId block_id) :
        type(type), block_id(block_id)
    {}
};

enum class SegType : uint8_t
{
    Main,
    Mission,
    Streamed,
    Dummy,      // must be the last enum value
};

// Try to make this struct as small as possible.
struct SegReference
{
    uint8_t     reserved;   //< (padding; for future use)
    SegType     segtype;    //< Type of segment
    uint16_t    segindex;   //< Index on specific segment array (e.g. mission_segments[seg_index])
    uint32_t    data_index; //< Index on std::vector<DecompiledData>

    explicit SegReference(uint8_t, SegType segtype, size_t segindex, uint32_t data_index) :
        segtype(segtype), segindex(static_cast<uint16_t>(segindex)), data_index(data_index)
    {}


    bool operator==(const SegReference& rhs) const
    {
        return this->data_index == rhs.data_index
            && this->segindex == rhs.segindex
            && this->segtype == rhs.segtype;
    }
    
    bool operator!=(const SegReference& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<(const SegReference& rhs) const
    {
        if(this->segtype != rhs.segtype)
            return (static_cast<uint8_t>(this->segtype) < static_cast<uint8_t>(rhs.segtype));

        if(this->segindex != rhs.segindex)
            return (this->segindex < rhs.segindex);

        return (this->data_index < rhs.data_index);
    }
};

struct BlockList;

struct Block
{
    SegReference block_begin;
    size_t       length;

    std::vector<BlockId> pred;   //< Predecessor blocks
    std::vector<BlockId> succ;   //< Successor blocks

    explicit Block(SegReference block_begin, size_t length) :
        block_begin(std::move(block_begin)), length(length)
    {}

    using iterator = const DecompiledData*;
    using const_iterator = iterator;

    const_iterator begin(const BlockList& bl) const;
    const_iterator end(const BlockList& bl) const;

    iterator begin(const BlockList& bl) { return const_cast<const Block&>(*this).begin(bl); }
    iterator end(const BlockList& bl)   { return const_cast<const Block&>(*this).end(bl); }
};

// TODO make explicit constructor
struct BlockList
{
    using block_range = std::pair<BlockId, BlockId>;

    std::vector<Block>              blocks;             // dummy blocks (at the end) aren't sorted, so container isn't
                                                        // see ranges below for sorted ranges

    block_range                     non_dummy_blocks;   // all blocks in this range are guaranted to be sorted by offset
    block_range                     main_blocks;        // all blocks in this range are guaranted to be sorted by offset
    std::vector<block_range>        mission_blocks;     // all blocks in this range are guaranted to be sorted by offset

    std::vector<ProcEntries>        proc_entries; // sorted by offset

//protected:
    const Disassembler&               main_segment;
    const std::vector<Disassembler>&  mission_segments;

public:
    using iterator = Block*;
    using const_iterator = const iterator;

    auto begin()        { return this->blocks.data() + 0; }
    auto end()          { return this->blocks.data() + non_dummy_blocks.second; }
    auto begin() const  { return this->blocks.data() + 0; }
    auto end() const    { return this->blocks.data() + non_dummy_blocks.second; }

    Block& block(BlockId id)             { return this->blocks[id]; }
    const Block& block(BlockId id) const { return this->blocks[id]; }

    optional<ProcEntries&> proc(BlockId id)
    {
        for(auto& entry : proc_entries)
        {
            if(entry.block_id == id)
                return entry;
        }
        return nullopt;
    }


    auto segref_to_data(const SegReference& segref) const -> const DecompiledData*
    {
        if(segref.segtype == SegType::Main)
            return main_segment.get_data().data() + segref.data_index;
        else if(segref.segtype == SegType::Mission)
            return mission_segments[segref.segindex].get_data().data() + segref.data_index;
        else
            Unreachable();
    }

    void link_blocks(BlockId link_from, BlockId link_to)
    {
        this->blocks[link_from].succ.emplace_back(link_to);
        this->blocks[link_to].pred.emplace_back(link_from);
    }

    void link_script_spawn(BlockId spawner_proc, BlockId spawned_proc)
    {
        auto& spawner = this->proc(spawner_proc).value(); // TODO what if nullopt
        auto& spawned = this->proc(spawned_proc).value(); // TODO what if nullopt

        spawner.spawns_script.emplace_back(spawned_proc);
        spawned.spawned_from.emplace_back(spawner_proc);
    }

    void link_call(BlockId caller_proc, BlockId called_proc)
    {
        auto& caller = this->proc(caller_proc).value(); // TODO what if nullopt
        auto& called = this->proc(called_proc).value(); // TODO what if nullopt

        caller.calls_into.emplace_back(called_proc);
        called.called_from.emplace_back(caller_proc);
    }



    optional<BlockId> block_from_label(const SegReference& current_seg, int32_t label_target)
    {
        if(label_target < 0 && current_seg.segtype == SegType::Main)
        {
            label_target = -label_target;
        }

        if(label_target >= 0)
        {
            for(BlockId block_id = main_blocks.first; block_id != main_blocks.second; ++block_id)
            {
                size_t x = this->blocks[block_id].block_begin.data_index;
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

protected:
    friend BlockList find_basic_blocks(const Commands& commands, const Disassembler& main_segment,
                                       const std::vector<Disassembler>& mission_segments);

    static void find_ranges(const std::vector<Block>& blocks, block_range& main_blocks, std::vector<block_range>& mission_blocks);
};

auto Block::begin(const BlockList& bl) const -> Block::const_iterator
{
    return bl.segref_to_data(this->block_begin);
}

auto Block::end(const BlockList& bl) const -> Block::const_iterator
{
    return this->begin(bl) + this->length;
}

template<typename Visitor>
inline bool depth_first_internal_(dynamic_bitset& visited, const BlockList& block_list, BlockId block, bool forward, Visitor&& visitor)
{
    assert(visited[block] == false);
    visited[block] = true;

    if(!visitor(block))
        return false;

    for(BlockId next : (forward? block_list.blocks[block].succ : block_list.blocks[block].pred))
    {
        if(!visited[next])
        {
            if(!depth_first_internal_(visited, block_list, next, forward, visitor))
                return false;
        }
    }

    return true;
}

template<typename Visitor>
inline void depth_first(const BlockList& block_list, BlockId start_block, bool forward, Visitor visitor)
{
    dynamic_bitset visited(block_list.blocks.size());
    depth_first_internal_(visited, block_list, start_block, forward, std::move(visitor));
}

inline optional<std::string> find_script_name(const Commands& commands, const BlockList& block_list, BlockId start_block)
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

inline
BlockList find_basic_blocks(const Commands& commands, const Disassembler& main_segment, const std::vector<Disassembler>& mission_segments)
{
    struct ProcEntries1
    {
        ProcType     type;
        SegReference seg_entry;
    };

    using ForwardEdge = std::pair<SegReference, SegReference>; // jumps from .first to .second

    std::vector<SegReference> leaders;          // **All** possible labels / start of basic blocks.
    std::vector<ProcEntries1> proc_entries1;    // Entry points, including for gosubs
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
                return segref_by_loffset(mission_segments[*opt_imm32], SegType::Mission, 0, 0);
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
                proc_entries1.emplace_back(ProcEntries1 { ProcType::Gosub, segref });
            }
            else if(dcmd.id == commands.start_new_script().id)
            {
                Expects(dcmd.args.size() >= 1);
                auto segref = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                leaders.emplace_back(segref);
                //script_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntries1{ ProcType::Script, segref });
            }
            else if(dcmd.id == commands.launch_mission().id)
            {
                Expects(dcmd.args.size() == 1);
                auto segref = segref_by_label(segment_asm, segtype, segindex, dcmd.args[0]);
                leaders.emplace_back(segref);
                //subscript_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntries1{ ProcType::Subscript, segref });
            }
            else if(dcmd.id == commands.load_and_launch_mission().id)
            {
                Expects(dcmd.args.size() == 1);
                auto segref = segref_by_mission(dcmd.args[0]);
                leaders.emplace_back(segref);
                //mission_entry.emplace_back(segref);
                proc_entries1.emplace_back(ProcEntries1{ ProcType::Mission, segref });
            }
            // TODO more (cutscene thing, etc)
            // TODO make a assert (debug-only) regarding cmd having a label param and we didn't handle it
        }
    };


    // Put main entry point on leaders and proc entries
    {
        auto segref_start = SegReference{ 0, SegType::Main, 0, 0 };
        leaders.emplace_back(segref_start);
        proc_entries1.emplace_back(ProcEntries1{ ProcType::Main, segref_start });
    }

    find_leaders(main_segment, SegType::Main, 0);
    for(size_t i = 0; i < mission_segments.size(); ++i)
        find_leaders(mission_segments[i], SegType::Mission, i);



    // Leaders (and thus Blocks) must be sorted by absolute offset.
    std::sort(leaders.begin(), leaders.end());
    leaders.erase(std::unique(leaders.begin(), leaders.end()), leaders.end());


    //
    std::vector<Block> blocks;
    std::vector<ProcEntries> proc_entries;

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

        /*
        if(blocks.back().block_end.segindex == 0 && blocks.back().block_end.data_index == 0)
        {
            __debugbreak();
        }
        */

        if(begin != end)
        {
            auto proc_it = std::find_if(proc_entries1.begin(), proc_entries1.end(), [&](const ProcEntries1& x) {
                return x.seg_entry == begin;
            });
            if(proc_it != proc_entries1.end())
            {
                proc_entries.emplace_back(ProcEntries { proc_it->type, blocks.size() - 1 });
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

inline
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

inline
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

                    block_list.link_call(entry_point.block_id, target_block_id);
                }
                else if(dcmd.id == commands.start_new_script().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_script_spawn(entry_point.block_id, target_block_id);
                }
                else if(dcmd.id == commands.launch_mission().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_script_spawn(entry_point.block_id, target_block_id);
                }
                else if(dcmd.id == commands.load_and_launch_mission().id)
                {
                    int32_t target_label = get_imm32(dcmd.args[0]).value(); // TODO what if not a imm32
                    BlockId target_block_id = block_list.block_from_label(current_seg, target_label).value(); // TODO what if fails

                    block_list.link_script_spawn(entry_point.block_id, target_block_id);
                }
            }

            return true;
        });
    }
}



inline
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



