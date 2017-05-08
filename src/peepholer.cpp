#include <stdinc.h>
#include "codegen.hpp"

constexpr size_t MAX_BRANCH_INDIRECTION = 4;

// TODO replace conditions which are always true with no check.
// TODO constant folding.

static auto build_label_assoc(const std::vector<CompiledData>& data) -> std::unordered_map<shared_ptr<Label>, size_t>
{
    std::unordered_map<shared_ptr<Label>, size_t> map;

    for(size_t i = 0; i < data.size(); ++i)
    {
        auto& d = data[i].data;
        if(is<CompiledLabelDef>(d))
            map.emplace(get<CompiledLabelDef>(d).label, i);
    }

    return map;
}

static auto get(std::vector<CompiledData>& c, size_t index) -> optional<CompiledCommand&>
{
    if(index < c.size())
    {
        auto& r = c[index];
        if(is<CompiledCommand>(r.data))
            return (get<CompiledCommand>(r.data));
    }
    return nullopt;
}

static auto get_arg(CompiledCommand& ccmd, size_t i) -> optional<ArgVariant&>
{
    if(i < ccmd.args.size())
        return (ccmd.args[i]);
    return nullopt;
}

static auto get_label(CompiledCommand& ccmd, size_t i) -> shared_ptr<Label>
{
    if(auto opt = get_arg(ccmd, i))
    {
        if(is<shared_ptr<Label>>(*opt))
            return get<shared_ptr<Label>>(*opt);
    }
    return nullptr;
}

void CodeGenerator::peepholer(ProgramContext& program)
{
    if(!program.opt.optimize_with_peepholer)
        return;

	const Commands& commands = program.commands;

    size_t current_basic_block = 0;

    auto& labels_map = build_label_assoc(this->compiled);

    auto get_op_at = [&](const shared_ptr<Label>& label) -> optional<CompiledCommand&>
    {
        if(label != nullptr)
        {
            auto it = labels_map.find(label);
            if(it != labels_map.end())
            {
                for(auto i = it->second; i < this->compiled.size(); ++i)
                {
                    if(is<CompiledLabelDef>(this->compiled[i].data))
                        continue;
                    else if(is<CompiledCommand>(this->compiled[i].data))
                        return (get<CompiledCommand>(this->compiled[i].data));
                    else
                        break;
                }
            }
        }
        return nullopt;
    };

    for(size_t i = 0; i < this->compiled.size(); ++i)
    {
        if(is<CompiledLabelDef>(this->compiled[i].data))
        {
            ++current_basic_block;
			continue;
        }

        if(auto opt_op0 = get(this->compiled, i))
        {
			auto& op0 = *opt_op0;

            // The following optimization will open doors to more GOTO related optimizations, so keep it at the top.
            // Replace {GOTO_IF_FALSE -> GOTO -> .. -> BLOCK} with {GOTO_IF_FALSE -> BLOCK}.
            // Replace {GOTO -> GOTO -> .. -> BLOCK} with {GOTO -> BLOCK}.
			if(commands.equal(*op0.command, commands.goto_if_false)
            || commands.equal(*op0.command, commands.goto_))
			{
                shared_ptr<Label> next_target = get_label(op0, 0);

                for(size_t indir_count = 0; indir_count < MAX_BRANCH_INDIRECTION; ++indir_count)
                {
                    if(auto opt_op1 = get_op_at(next_target))
                    {
                        if(commands.equal(*opt_op1->command, commands.goto_))
                            next_target = get_label(*opt_op1, 0);
                        else
                            break;
                    }
                    else
                        break;
                }

                if(next_target)
                {
                    op0.args[0] = next_target;
                }
			}

            // Replace {GOTO -> RETURN} with {RETURN}
            // Replace {GOTO -> CLEO_RETURN} with {CLEO_RETURN}
            if(commands.equal(*op0.command, commands.goto_))
            {
                auto op0_target = get_label(op0, 0);
                if(auto opt_op1 = get_op_at(op0_target))
                {
                    auto& op1 = *opt_op1;
                    if(commands.equal(*op1.command, commands.return_)
                    || commands.equal(*op1.command, commands.cleo_return))
                    {
                        op0 = op1;
                    }
                }
            }


            // This must be at the bottom.
            // Remove any code past unconditional branches in the same basic block.
            if(commands.equal(*op0.command, commands.goto_)
            || commands.equal(*op0.command, commands.return_)
            || commands.equal(*op0.command, commands.cleo_return)
            || commands.equal(*op0.command, commands.terminate_this_script)
            || commands.equal(*op0.command, commands.terminate_this_custom_script))
            {
                for(i = i+1; i < this->compiled.size(); ++i)
                {
                    if(!is<CompiledLabelDef>(this->compiled[i].data))
                        this->compiled[i] = CompiledData(CompiledNothing{});
                    else
                        break;
                }

                --i;
                continue;
            }
        }
    }
}
