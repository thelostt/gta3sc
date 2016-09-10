// TODO doc comments
#pragma once
#include "stdinc.h"
#include "disassembler.hpp"
#include "flow.hpp"

struct DecompilerContext2;

template<typename T>
std::string decompile_data(const T&, size_t index, DecompilerContext2&);
std::string decompile_data(const DecompiledData&, size_t index, DecompilerContext2&);

struct DecompilerContext2
{
private:
    std::vector<DecompiledData> data;
public://

protected:
    friend std::string decompile_data(const DecompiledCommand& ccmd, size_t, DecompilerContext2& context);
    friend std::string decompile_data(const DecompiledLabelDef& label, size_t, DecompilerContext2& context);

    const Commands& commands;

    size_t unique_id; //< Helper for labels
    std::string script_name;

    const BlockList& block_list;
    std::vector<BlockList::Loop> loops;

    BlockList::block_range block_range;

    SegType  segtype;
    uint16_t segindex;

    // TODO something else than map
    std::map<const StatementNode*, bool> generated;

    size_t num_tabs = 0;

public:
    // TODO this takes a copy of the vector<DecompiledData>, maybe take ref?
    DecompilerContext2(const Commands& commands, std::vector<DecompiledData> decompiled,
                       const BlockList& block_list,
                       SegType segtype, size_t segindex)
        : commands(commands), data(std::move(decompiled)),
          block_list(block_list),
          segtype(segtype), segindex(segindex)
    {
        this->block_range = this->block_list.get_block_range(this->segtype, this->segindex).value();

        switch(segtype)
        {
            case SegType::Main:
                unique_id = 0;
                break;
            case SegType::Mission:
                unique_id = 1 + segindex;
                break;
            default:
                Unreachable();
        }
    }

    std::string decompile()
    {
        std::string output;

        /*
        for(size_t i = 0; i < this->data.size(); ++i)
        {
            auto& d = this->data[i];
            output += ::decompile_data(d, i, *this);
        }
        */

        this->loops = find_natural_loops(this->block_list, this->block_range);
        sort_natural_loops(this->block_list, this->loops);

        for(auto& proc_entry : block_list.proc_entries)
        {
            if(proc_entry.block_id >= this->block_range.first
                && proc_entry.block_id < this->block_range.second)
            {
                auto entry_node = to_statements(block_list, proc_entry.block_id);
                entry_node = structure_dowhile(block_list, entry_node, loops);
                entry_node = structure_groups(block_list, entry_node);
                entry_node = structure_ifs(block_list, entry_node);
                output = this->generate_source(entry_node);
            }
        }

        return output;
    }

    optional<const Block&> get_block(size_t data_index)
    {
        for(BlockId id = block_range.first; id != block_range.second; ++id)
        {
            auto& block = block_list.block(id);
            if(block.block_begin.segtype == this->segtype
             && block.block_begin.segindex == this->segindex
             && block.block_begin.data_index == data_index)
            {
                return block;
            }
        }
        return nullopt;
    }

    std::string generate_source(const shared_ptr<StatementNode>& node);
    std::string generate_source(const shared_ptr<StatementBlock>& node);
    std::string generate_source(const shared_ptr<StatementGroup>& node);
    std::string generate_source(const shared_ptr<StatementWhile>& node);
    std::string generate_source(const shared_ptr<StatementIf>& node);
    std::string generate_source(const Block& block, size_t from, size_t until);
    std::string generate_conditions(const shared_ptr<StatementBlock>& node, size_t andor_pos);

    void enter_identation()
    {
        ++num_tabs;
    }

    void exit_identation()
    {
        assert(num_tabs > 0);
        --num_tabs;
    }

    std::string line(const std::string& xline)
    {
        std::string output;
        output.reserve(num_tabs + xline.size());
        output.assign(num_tabs, '\t');
        output.append(xline);
        return output;
    }
};


inline std::string decompile_data(const EOAL&, size_t index, DecompilerContext2&)
{
    return std::string();
}

inline std::string decompile_data(const int8_t& value, size_t index, DecompilerContext2&)
{
    return std::to_string(static_cast<int>(value)); // avoid charish behaviour
}

inline std::string decompile_data(const int16_t& value, size_t index, DecompilerContext2&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const int32_t& value, size_t index, DecompilerContext2&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const float& value, size_t index, DecompilerContext2&)
{
    return std::to_string(value);
}

inline std::string decompile_data(const DecompiledString& str, size_t index, DecompilerContext2&)
{
    std::string output;
    char quotes = 0;

    switch(str.type)
    {
        case CompiledString::Type::TextLabel8:
        case CompiledString::Type::TextLabel16:
            quotes = 0;
            output.reserve(str.storage.size());
            break;
        case CompiledString::Type::StringVar:
        case CompiledString::Type::String128:
            quotes = '"';
            output.reserve(str.storage.size() + 2 + 4); // +2 for quotes; +4 for possible escapes;
            break;
    }

    if(quotes)
        output.push_back(quotes);

    // TODO parse to escape sequences
    for(size_t i = 0; i < str.storage.size(); ++i)
    {
        if(str.storage[i] == '\0') break;
        output.push_back(str.storage[i]);
    }

    if(quotes)
        output.push_back(quotes);

    return output;
}

inline std::string decompile_data(const DecompiledVar& v, size_t index, DecompilerContext2&)
{
    std::string output;
    bool adma = (v.offset % 4) != 0;

    output.push_back(v.global? 'g' : 'l');
    if(adma)
    {
        output.push_back('x');
        output.append(std::to_string(v.offset));
    }
    else
    {
        output.append(std::to_string(v.offset / 4));
    }
    return output;
}

inline std::string decompile_data(const DecompiledVarArray& v, size_t index, DecompilerContext2& context)
{
    std::string output;
    output += decompile_data(v.base, index, context);
    output.push_back('[');
    output += decompile_data(v.index, index, context);
    output.push_back(']');
    return output;
}

inline std::string decompile_data(const ArgVariant2& varg, size_t index, DecompilerContext2& context)
{
    return visit_one(varg, [&](const auto& arg) { return ::decompile_data(arg, index, context); });
}

inline std::string decompile_data(const DecompiledCommand& ccmd, size_t index, DecompilerContext2& context)
{
    std::string output;

    bool not_flag = (ccmd.id & 0x8000) != 0;

    auto cmd_name = *context.commands.find_command_name(ccmd.id & 0x7FFF, true);

    output.reserve(cmd_name.size() + ccmd.args.size() * 6);

    if(not_flag) output += "NOT ";
    output += cmd_name;
    output.push_back(' ');
    for(auto& arg : ccmd.args)
    {
        output += ::decompile_data(arg, index, context);
        output.push_back(' ');
    }

    if(cmd_name == "SCRIPT_NAME" && ccmd.args.size() >= 1)
    {
        context.script_name = get_immstr(ccmd.args[0]).value_or("");
    }

    output.push_back('\n');
    return context.line(output);
}

inline std::string decompile_data(const DecompiledLabelDef& label, size_t data_index, DecompilerContext2& context)
{
    std::string output;

#if 0
    if(auto opt_block = context.get_block(data_index))
    {
        const Block& block = *opt_block;
        BlockId block_id = context.block_list.block_id(block);

        output += fmt::format("\n// BLOCK ID: {}", block_id);
        output += "\n// DOMINATORS: ";
        for(size_t k = context.block_range.first; k < context.block_range.second; ++k)
        {
            if(block.dominators[k])
            {
                output += fmt::format("{} ", k);
            }
        }

        for(auto& loop : context.loops)
        {
            if(loop.head == block_id)
            {
                output += fmt::format("\n// HEADER OF A LOOP {}!", (void*) &loop);
            }

            if(loop.tail == block_id)
            {
                output += fmt::format("\n// TAIL OF A LOOP {}!", (void*) &loop);
            }
        }
    }
    else
    {
        Unreachable();
        output += "\n//BLOCK ID NOT FOUND";
    }
#endif

    if(context.script_name.empty())
    {
        if(context.unique_id == 0)
            output += fmt::format("\nLABEL_{}:\n", label.offset);
        else
            output += fmt::format("\nLABEL_{}_{}:\n", context.unique_id, label.offset);
    }
    else
    {
        output += fmt::format("\n{}_{}:\n", context.script_name, label.offset);
    }

    return context.line(output);
}

inline std::string decompile_data(const DecompiledHex& hex, size_t index, DecompilerContext2& context)
{
    std::string output;
    output.reserve(sizeof("\nHEX\n") + (hex.data.size() * 3) + sizeof("\nENDHEX\n\n") + 32);

    output += context.line("\nHEX\n");
    for(auto& x : hex.data)
    {
        char buffer[3 + 1];
        snprintf(buffer, sizeof(buffer), "%.2X ", x);
        output.append(std::begin(buffer), std::end(buffer) - 1);
    }
    output += context.line("\nENDHEX\n\n");
    return output;
}

inline std::string decompile_data(const DecompiledData& data, size_t index, DecompilerContext2& context)
{
    std::string output;
    output = visit_one(data.data, [&](const auto& data) { return ::decompile_data(data, index, context); });
    return output;
}

inline std::string DecompilerContext2::generate_source(const shared_ptr<StatementNode>& node)
{
    using std::static_pointer_cast;

    if(this->generated[node.get()])
        return "";

    this->generated[node.get()] = true;

    switch(node->type)
    {
        case StatementNode::Type::Block:
            return generate_source(static_pointer_cast<StatementBlock>(node));
        case StatementNode::Type::Group:
            return generate_source(static_pointer_cast<StatementGroup>(node));
        case StatementNode::Type::While:
            return generate_source(static_pointer_cast<StatementWhile>(node));
        case StatementNode::Type::If:
            return generate_source(static_pointer_cast<StatementIf>(node));
        case StatementNode::Type::Break:
            return this->line("BREAK\n");
        default:
            Unreachable();
    }
}

inline std::string DecompilerContext2::generate_source(const shared_ptr<StatementBlock>& node)
{
    const Block& block = this->block_list.block(node->block_id);

    // TODO check kind of branch instead of count of successors
    if(node->succ.size() == 0)
    {
        return this->generate_source(block, node->block_from, node->block_until);
    }
    else if(node->succ.size() == 1)
    {
        std::string output = this->generate_source(block, node->block_from, node->block_until);
        output += this->generate_source(node->succ[0]);
        return output;
    }
    else if(node->succ.size() == 2)
    {
        std::string output =  this->generate_source(block, node->block_from, node->block_until);
        output += this->generate_source(node->succ[1]); // case branch not taken
        output += this->generate_source(node->succ[0]); // case branch taken
        return output;
    }
    else
    {
        Unreachable();
    }
}

inline std::string DecompilerContext2::generate_source(const shared_ptr<StatementGroup>& node)
{
    std::string output;
    auto it = node->first;
    while(true)
    {
        output += generate_source(it);
        if(it == node->last)
            break;
        else
            it = it->succ[0];
    }
    if(node->succ.size())
        output += generate_source(node->succ[0]);
    return output;
}

inline std::string DecompilerContext2::generate_source(const shared_ptr<StatementWhile>& node)
{
    using std::static_pointer_cast;
    std::string output;
    if(node->has_break_node())
    {
        {
            auto& b = block_list.block(node->loop_head->block_id);
            if(b.length >= 1) output += generate_source(b, 0, b.length - 1);
        }

        output += this->line("WHILE ");
        this->generated[node->loop_head.get()] = true;
        output += generate_conditions(node->loop_head, 1);
        this->enter_identation();
        output += generate_source(node->loop_head->succ[1]);
        this->exit_identation();
        output += this->line("ENDWHILE\n");
        output += generate_source(node->break_node());
    }
    else
    {
        // Expects no conditions
        this->enter_identation();
        this->generated[node->loop_head.get()] = true;
        output += generate_source(node->loop_head);
        this->exit_identation();
    }
    return output;
}

inline std::string DecompilerContext2::generate_source(const shared_ptr<StatementIf>& node)
{
    using std::static_pointer_cast;
    std::string output;
    output += this->line("IF ");
    output += generate_conditions(node->cond_block, node->andor_pos);
    this->enter_identation();
    output += generate_source(node->true_block);
    this->exit_identation();
    if(node->false_block)
    {
        output += this->line("ELSE");
        this->enter_identation();
        output += generate_source(node->false_block);
        this->exit_identation();
    }
    output += this->line("ENDIF\n");
    if(node->succ.size())
        output += generate_source(node->succ[0]);
    return output;
}

inline std::string DecompilerContext2::generate_source(const Block& block, size_t from, size_t until)
{
    std::string output;
    for(auto it = block.begin(block_list) + from, end = block.end(block_list) - until; it != end; ++it)
    {
        output += decompile_data(*it, -1, *this);
    }
    return output;
}

inline std::string DecompilerContext2::generate_conditions(const shared_ptr<StatementBlock>& node, size_t andor_pos)
{
    const Block& block = this->block_list.block(node->block_id);

    std::string output;

    const auto& dcmd = get<DecompiledCommand>((block.begin(block_list) + andor_pos)->data);
    size_t argc = get_imm32(dcmd.args[0]).value();
    bool first_iter = true;
    auto saved_num_tabs = this->num_tabs;

    for(auto it = block.begin(block_list) + andor_pos + 1, end = block.end(block_list) - 1; it != end; ++it)
    {
        if(!first_iter)
            output += (argc >= 1 && argc <= 8)? "AND " : "OR ";
        else
            this->num_tabs = 0;
        
        output += decompile_data(*it, -1, *this);

        if(first_iter)
        {
            first_iter = false;
            this->num_tabs = saved_num_tabs;
        }
    }
    return output;

}
