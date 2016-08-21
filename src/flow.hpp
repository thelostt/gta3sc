#pragma once
#include "disassembler.hpp"

struct tag_call_graph_t {};
constexpr tag_call_graph_t call_graph = {};

struct tag_spawn_graph_t {};
constexpr tag_spawn_graph_t spawn_graph = {};

using BlockId = size_t;
using ProcId  = size_t;

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

struct ProcEntry
{
    struct XRefInfo
    {
        BlockId block_id;
        ProcId  proc_id;
    };

    ProcType           type;
    BlockId            block_id;
    optional<BlockId>  exit_block;    //< Available after find_edges

    std::vector<XRefInfo> calls_into;
    std::vector<XRefInfo> called_from;

    std::vector<XRefInfo> spawns_script;
    std::vector<XRefInfo> spawned_from;

    explicit ProcEntry(ProcType type, BlockId block_id) :
        type(type), block_id(block_id)
    {}
};

enum class SegType : uint8_t
{
    Main,
    Mission,
    Streamed,
    ExitNode,      // must be the last enum value because its the last kind of block
                   // inserted into the vector (which is sorted by segtype)
};

// Try to make this struct as small as possible.
struct SegReference
{
    uint8_t     reserved;   //< (padding; for future use)
    SegType     segtype;    //< Type of segment
    uint16_t    segindex;   //< Index on specific segment array (e.g. mission_segments[seg_index])
    uint32_t    data_index; //< Index on std::vector<DecompiledData>

    explicit SegReference(uint8_t, SegType segtype, size_t segindex, size_t data_index) :
        segtype(segtype), segindex(static_cast<uint16_t>(segindex)), data_index(static_cast<uint32_t>(data_index))
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

    dynamic_bitset       dominators; //< Available after compute_dominators. Which blocks dominates this block.

    explicit Block(SegReference block_begin, size_t length) :
        block_begin(std::move(block_begin)), length(length)
    {}

    using iterator = const DecompiledData*;
    using const_iterator = iterator;

    const_iterator begin(const BlockList& bl) const;
    const_iterator end(const BlockList& bl) const;

    iterator begin(const BlockList& bl) { return const_cast<const Block&>(*this).begin(bl); }
    iterator end(const BlockList& bl)   { return const_cast<const Block&>(*this).end(bl); }

    // i.e. block that contains a RETURN / TERMINATE_THIS_SCRIPT at the end, which next linked block is dummy.
    bool is_pre_end_block(const BlockList& bl) const;
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

    std::vector<ProcEntry>        proc_entries; // sorted by offset

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

    ProcEntry& proc(ProcId id)             { return this->proc_entries[id]; }
    const ProcEntry& proc(ProcId id) const { return this->proc_entries[id]; }

    optional<ProcEntry&> find_proc_by_entry(BlockId id)
    {
        for(auto& entry : proc_entries)
        {
            if(entry.block_id == id)
                return entry;
        }
        return nullopt;
    }

    BlockId block_id(const Block& block) const
    {
        assert(std::addressof(block) >= blocks.data()
            && std::addressof(block) < blocks.data() + blocks.size());
        return std::addressof(block) - blocks.data();
    }

    ProcId proc_id(const ProcEntry& proc_entry) const
    {
        assert(std::addressof(proc_entry) >= proc_entries.data()
            && std::addressof(proc_entry) < proc_entries.data() + proc_entries.size());
        return std::addressof(proc_entry) - proc_entries.data();
    }


    auto segref_to_data(const SegReference& segref) const -> const DecompiledData*
    {
        if(segref.segtype == SegType::Main)
            return main_segment.get_data().data() + segref.data_index;
        else if(segref.segtype == SegType::Mission)
            return mission_segments[segref.segindex].get_data().data() + segref.data_index;
        else if(segref.segtype == SegType::ExitNode)
            return nullptr;
        else
            Unreachable();
    }

    void link_blocks(BlockId link_from, BlockId link_to)
    {
        this->blocks[link_from].succ.emplace_back(link_to);
        this->blocks[link_to].pred.emplace_back(link_from);
    }

    void link_script_spawn(BlockId spawner_block, ProcEntry& spawner, ProcEntry& spawned)
    {
        using XRefInfo = ProcEntry::XRefInfo;
        spawner.spawns_script.emplace_back(XRefInfo { spawner_block, proc_id(spawned) });
        spawned.spawned_from.emplace_back(XRefInfo { spawner_block, proc_id(spawner) });
    }

    void link_call(BlockId caller_block, ProcEntry& caller, ProcEntry& called)
    {
        using XRefInfo = ProcEntry::XRefInfo;
        caller.calls_into.emplace_back(XRefInfo { caller_block, proc_id(called) });
        called.called_from.emplace_back(XRefInfo { caller_block, proc_id(caller) });
    }



    optional<BlockId> block_from_label(const SegReference& current_seg, int32_t label_target);
    optional<BlockId> block_from_mission(int32_t mission_id);

    optional<block_range> get_block_range(SegType segtype, uint16_t segindex) const;

protected:
    friend BlockList find_basic_blocks(const Commands& commands, const Disassembler& main_segment,
                                       const std::vector<Disassembler>& mission_segments);

    static void find_ranges(const std::vector<Block>& blocks, block_range& main_blocks, std::vector<block_range>& mission_blocks);
};

inline auto Block::begin(const BlockList& bl) const -> Block::const_iterator
{
    return bl.segref_to_data(this->block_begin);
}

inline auto Block::end(const BlockList& bl) const -> Block::const_iterator
{
    return this->begin(bl) + this->length;
}

// 
// BlockList building (call in this order)
//
BlockList find_basic_blocks(const Commands& commands, const Disassembler& main_segment, const std::vector<Disassembler>& mission_segments);
void find_edges(BlockList& block_list, const Commands& commands);
void find_call_edges(BlockList& block_list, const Commands& commands);
void compute_dominators(BlockList& block_list);

//
// Utility functions
//
optional<std::string> find_script_name(const Commands& commands, const BlockList& block_list, const ProcEntry& entry_point);
optional<std::string> find_script_name(const Commands& commands, const BlockList& block_list, BlockId start_block);


//
// Depth First on Control Flow
//
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


//
// Depth First on Call Graph and Spawn Graph
//
template<typename Tag, typename Visitor>
inline bool depth_first_internal_(Tag, dynamic_bitset& visited, const BlockList& block_list, const ProcEntry& proc, bool forward, Visitor&& visitor)
{
    static_assert(std::is_same<Tag, tag_call_graph_t>::value || std::is_same<Tag, tag_spawn_graph_t>::value, "wrong Tag.");

    size_t proc_id = block_list.proc_id(proc);

    assert(visited[proc_id] == false);
    visited[proc_id] = true;

    if(!visitor(proc))
        return false;

    const auto& iter_vec = std::is_same<Tag, tag_call_graph_t>::value? (forward? proc.calls_into : proc.called_from) :
                           /*std::is_same<Tag, tag_spawn_graph_t>::value?*/ (forward? proc.spawns_script : proc.spawned_from);

    for(auto& xref : iter_vec)
    {
        if(!visited[xref.proc_id])
        {
            auto& next = block_list.proc(xref.proc_id);
            if(!depth_first_internal_(Tag{}, visited, block_list, next, forward, visitor))
                return false;
        }
    }

    return true;
}

template<typename Visitor>
inline void depth_first(tag_call_graph_t tag, const BlockList& block_list, const ProcEntry& start_proc, bool forward, Visitor visitor)
{
    dynamic_bitset visited(block_list.proc_entries.size());
    depth_first_internal_(tag, visited, block_list, start_proc, forward, std::move(visitor));
}

template<typename Visitor>
inline void depth_first(tag_spawn_graph_t tag, const BlockList& block_list, const ProcEntry& start_proc, bool forward, Visitor visitor)
{
    dynamic_bitset visited(block_list.proc_entries.size());
    depth_first_internal_(tag, visited, block_list, start_proc, forward, std::move(visitor));
}
