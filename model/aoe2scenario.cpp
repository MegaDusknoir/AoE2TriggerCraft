
#include "aoe2scenario.h"

namespace AoE2ScenarioNamespace
{
    DeflateClass AoE2Scenario::deflate;
    AoE2Scenario::AoE2Scenario()
        :triggers(new TriggerManager(&this->scen)), text_io(new TextIO(&this->scen))
    {
    }
    AoE2Scenario::~AoE2Scenario()
    {
    }
    AoE2Scenario::AoE2Scenario(const char* file_path)
        :triggers(new TriggerManager(&this->scen)), text_io(new TextIO(&this->scen))
    {
        open(file_path);
    }
    AoE2Scenario::AoE2Scenario(const wchar_t* file_path)
        : triggers(new TriggerManager(&this->scen)), text_io(new TextIO(&this->scen))
    {
        open(file_path);
    }
    void AoE2Scenario::open(const char* file_path)
    {
        AutoFile fin(file_path, ios::in | ios::binary);
        open(fin);
    }
    void AoE2Scenario::open(const wchar_t* file_path)
    {
        AutoFile fin(file_path, ios::in | ios::binary);
        open(fin);
    }
	void AoE2Scenario::open(AutoFile& fin)
    {
        string check_version(4, 0);
        fin->read(reinterpret_cast<char*>(&check_version[0]), 4);
        if (check_version == string(current_version))
        {
#ifndef NSPEEDTRACE
            auto t1 = clock();
#endif
            fin->seekg(0, ios::end);
            auto file_size = static_cast<size_t>(fin->tellg());
            fin->seekg(ios::beg);
            string raw_binary(file_size, 0);
            fin->read(reinterpret_cast<char*>(&raw_binary[0]), file_size);
            fin->close();
            auto header_length = scen.header.read(raw_binary.data());
            string scen_body = raw_binary.substr(header_length);
            deflate_decompress(scen_body);
            auto raw_else(scen_body.substr(scen.body.read(scen_body.data())));
            if (raw_else.empty() == false)
            {
                throw scenario_parser_error("Unexpected file length.");
            }
            triggers->load();
#ifndef NSPEEDTRACE
            auto t2 = clock();
            std::cout << "Read completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
        }
        //else if (check_version == string("1.41"))
        //{
        //    //Transform 1.41 to newest format.
        //}
        else
        {
            string err_what;
            err_what = err_what + "Version " + check_version + " not supported yet.";
            throw scenario_version_error(err_what, string(current_version), check_version);
        }
    }

    void AoE2Scenario::save(const char* file_path)
    {
        AutoFile fout(file_path, ios::out | ios::binary);
        save(fout);
    }
    void AoE2Scenario::save(const wchar_t* file_path)
    {
        AutoFile fout(file_path, ios::out | ios::binary);
        save(fout);
    }
    void AoE2Scenario::save(AutoFile& fout)
    {
#ifndef NSPEEDTRACE
        auto t1 = clock();
#endif
        triggers->confirm();
        string shead(1 * 1024 * 1024, '\0');
        string sbody(16 * 1024 * 1024, '\0');
        shead.resize(scen.header.write(&shead[0]));
        sbody.resize(scen.body.write(&sbody[0]));
        deflate_compress(sbody);
        fout->write(shead.data(), shead.size());
        fout->write(sbody.data(), sbody.size());
        fout->close();
#ifndef NSPEEDTRACE
		auto t2 = clock();
		std::cout << "Write completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
    }

    void AoE2Scenario::deflate_decompress(string& raw)
    {
        //simple scenario may has larger compression ratio, so at least 4MiB
		size_t out_buf_size = std::max(raw.size() * 128, size_t(4 * 1024 * 1024));
        string out(out_buf_size, '\0');
        size_t out_size = deflate.decompress(raw.data(), raw.size(), static_cast<void *>(&out[0]), out.size());
        out.resize(out_size);
        raw = std::move(out);
    }
    void AoE2Scenario::deflate_compress(string& raw)
    {
        string out(raw.size() * 32, '\0');
        size_t out_size = deflate.compress(raw.data(), raw.size(), static_cast<void*>(&out[0]), out.size());
        out.resize(out_size);
        raw = std::move(out);
    }
    void AoE2Scenario::undo(void)
    {
        if (operator_backward.empty() == false)
        {
            operator_backward.back()->undo();
            operator_forward.push_back(std::move(operator_backward.back()));
            operator_backward.pop_back();
        }
    }
    void AoE2Scenario::redo(void)
    {
        if (operator_forward.empty() == false)
        {
            operator_forward.back()->redo();
            operator_backward.push_back(std::move(operator_forward.back()));
            operator_forward.pop_back();
        }
    }
    iterTrigger AoE2Scenario::add_trigger(int32_t location)
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorAddTrigger(*this, location)));
        operator_forward.clear();
        return dynamic_cast<OperatorAddTrigger*>(operator_backward.back().get())->get_return();
    }
    iterTrigger AoE2Scenario::delete_trigger(int32_t front, int32_t back)
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorDelTrigger(*this, front, back)));
        operator_forward.clear();
        return dynamic_cast<OperatorDelTrigger*>(operator_backward.back().get())->get_return();
    }
    void AoE2Scenario::sort_trigger()
    {
        operator_backward.push_back(unique_ptr<BaseOperator>(new OperatorSortTrigger(*this)));
        operator_forward.clear();
        //return dynamic_cast<OperatorSortTrigger*>(operator_backward.back().get())->get_return();
    }


    using ConditionStruct = AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct::ConditionStruct;
    using EffectStruct = AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct::EffectStruct;
    using TriggerStruct = AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct;
    using TriggerStructIdx = uint32_t;

    TriggerStruct& TriggerManager::operator[](size_t idx)
    {
        return scen->body.Triggers.trigger_data[list_by_order[idx]];
    }
    TriggerStruct& TriggerManager::at(size_t idx)
    {
        return (*this)[idx];
    }
    TriggerStructIdx TriggerManager::size()
    {
        return list_by_order.size();
    }
    int32_t TriggerManager::add_c(TriggerStructIdx parent_idx)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        parent.condition_data.push_back(ConditionStruct());
        parent.condition_display_order_array.push_back(parent.condition_data.size() - 1);
        return parent.condition_display_order_array.size() - 1;
    }
    int32_t TriggerManager::add_e(TriggerStructIdx parent_idx)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        parent.effect_data.push_back(EffectStruct());
        parent.effect_display_order_array.push_back(parent.effect_data.size() - 1);
        return parent.effect_display_order_array.size() - 1;
    }
    TriggerStructIdx TriggerManager::add()
    {
        scen->body.Triggers.trigger_data.push_back(TriggerStruct());
        list_by_order.push_back(scen->body.Triggers.trigger_data.size() - 1);
        return list_by_order.size() - 1;
    }
    TriggerStructIdx TriggerManager::add(const TriggerStruct& val)
    {
        scen->body.Triggers.trigger_data.push_back(val);
        list_by_order.push_back(scen->body.Triggers.trigger_data.size() - 1);
        return list_by_order.size() - 1;
    }
    TriggerStructIdx TriggerManager::add(TriggerStruct&& val)
    {
        scen->body.Triggers.trigger_data.push_back(std::move(val));
        list_by_order.push_back(scen->body.Triggers.trigger_data.size() - 1);
        return list_by_order.size() - 1;
    }
    void TriggerManager::del(TriggerStructIdx to_del)
    {
        del(list_by_order.begin() + to_del);
    }
    void TriggerManager::del_c(TriggerStructIdx parent_idx, int32_t to_del)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.condition_display_order_array;
        del_c(parent_idx, order.begin() + to_del);
    }
    void TriggerManager::del_e(TriggerStructIdx parent_idx, int32_t to_del)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.effect_display_order_array;
        del_e(parent_idx, order.begin() + to_del);
    }
    void TriggerManager::del(TriggerStructIdx to_del_begin, TriggerStructIdx n)
    {
        for (auto i = n; i != 0; --i)
        {
            del(to_del_begin);
        }
    }
    void TriggerManager::del(vector<TriggerStructIdx>::iterator to_del)
    {
        list_deleted.push_back(*to_del);
        list_by_order.erase(to_del);
    }
    void TriggerManager::del_c(TriggerStructIdx parent_idx, vector<int32_t>::iterator to_del)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.condition_display_order_array;
        //list_deleted.push_back(*to_del); // Todo: condition ver
        // // undoable del is a huge work, so temporary use this
        parent.condition_data.erase(parent.condition_data.begin() + *to_del);
        for (auto& cond_id : order)
        {
            if (cond_id > *to_del)
            {
                --cond_id;
            }
        }
        order.erase(to_del);
    }
    void TriggerManager::del_e(TriggerStructIdx parent_idx, vector<int32_t>::iterator to_del)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.effect_display_order_array;
        //list_deleted.push_back(*to_del); // Todo: effect ver
        // // undoable del is a huge work, so temporary use this
        parent.effect_data.erase(parent.effect_data.begin() + *to_del);
        for (auto& eff_id : order)
        {
            if (eff_id > *to_del)
            {
                --eff_id;
            }
        }
        order.erase(to_del);
    }
    void TriggerManager::mov_e(TriggerStructIdx parent_idx, int32_t target, int32_t idx_begin, int32_t idx_end)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.effect_display_order_array;
        mov_e(parent_idx, order.begin() + target, order.begin() + idx_begin, order.begin() + idx_end);
    }
    void TriggerManager::mov_e(TriggerStructIdx parent_idx, vector<int32_t>::iterator target, vector<int32_t>::iterator idx_begin, vector<int32_t>::iterator idx_end)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.effect_display_order_array;
        if (target < idx_begin || target > idx_end)
        {
            size_t target_idx;
            if (target < idx_begin)
            {
                target_idx = target - order.begin();
            }
            else
            {
                target_idx = target - (idx_end - idx_begin) - order.begin();
            }
            vector<TriggerStructIdx> operand(idx_begin, idx_end);
            order.erase(idx_begin, idx_end);
            order.insert(order.begin() + target_idx, operand.begin(), operand.end());
        }
    }
    void TriggerManager::mov_c(TriggerStructIdx parent_idx, int32_t target, int32_t idx_begin, int32_t idx_end)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.condition_display_order_array;
        mov_c(parent_idx, order.begin() + target, order.begin() + idx_begin, order.begin() + idx_end);
    }
    void TriggerManager::mov_c(TriggerStructIdx parent_idx, vector<int32_t>::iterator target, vector<int32_t>::iterator idx_begin, vector<int32_t>::iterator idx_end)
    {
        auto& parent = scen->body.Triggers.trigger_data[list_by_order[parent_idx]];
        auto& order = parent.condition_display_order_array;
        if (target < idx_begin || target > idx_end)
        {
            size_t target_idx;
            if (target < idx_begin)
            {
                target_idx = target - order.begin();
            }
            else
            {
                target_idx = target - (idx_end - idx_begin) - order.begin();
            }
            vector<TriggerStructIdx> operand(idx_begin, idx_end);
            order.erase(idx_begin, idx_end);
            order.insert(order.begin() + target_idx, operand.begin(), operand.end());
        }
    }
    void TriggerManager::mov(TriggerStructIdx target, TriggerStructIdx idx_begin, TriggerStructIdx idx_end)
    {
        mov(list_by_order.begin() + target, list_by_order.begin() + idx_begin, list_by_order.begin() + idx_end);
    }
    void TriggerManager::mov(vector<TriggerStructIdx>::iterator target, vector<TriggerStructIdx>::iterator idx_begin, vector<TriggerStructIdx>::iterator idx_end)
    {
        if (target < idx_begin || target > idx_end)
        {
            size_t target_idx;
            if (target < idx_begin)
            {
                target_idx = target - list_by_order.begin();
            }
            else
            {
                target_idx = target - (idx_end - idx_begin) - list_by_order.begin();
            }
            vector<TriggerStructIdx> operand(idx_begin, idx_end);
            list_by_order.erase(idx_begin, idx_end);
            list_by_order.insert(list_by_order.begin() + target_idx, operand.begin(), operand.end());
        }
    }
    void TriggerManager::sort_by_order()
    {
#ifndef NSPEEDTRACE
        auto t1 = clock();
#endif
        //scenario data don't include trigger id value, so generate
        vector<TriggerStructIdx> old_id_list;
        vector<TriggerStruct> new_trigger_data;
        old_id_list.resize(scen->body.Triggers.trigger_data.size());
        new_trigger_data.reserve(scen->body.Triggers.trigger_data.size());
        for (size_t i = 0; i < old_id_list.size(); ++i)
        {
            old_id_list[i] = i;
        }
        //reorder trigger vector by display order
        for (size_t i = 0; i < list_by_order.size(); ++i)
        {
            new_trigger_data.push_back(std::move(scen->body.Triggers.trigger_data[list_by_order[i]]));
            old_id_list[i] = list_by_order[i];
            list_by_order[i] = i;
        }
        //deleted trigger will be placed to the back
        for (size_t i = list_by_order.size(); i < old_id_list.size(); ++i)
        {
            new_trigger_data.push_back(std::move(scen->body.Triggers.trigger_data[list_deleted[i - list_by_order.size()]]));
            old_id_list[i] = list_deleted[i - list_by_order.size()];
            list_deleted[i - list_by_order.size()] = i;
        }
        scen->body.Triggers.trigger_data = std::move(new_trigger_data);
        //repoint old id call to new
        for (auto& trigger : scen->body.Triggers.trigger_data)
        {
            for (auto& effect : trigger.effect_data)
            {
                if (effect.trigger_id != -1)
                {
                    for (size_t i = 0; i < old_id_list.size(); ++i)
                    {
                        if (effect.trigger_id == old_id_list[i])
                        {
                            if (old_id_list[i] != i)
                            {
                                effect.trigger_id = i;
                            }
                            break;
                        }
                    }
                }
            }
        }
#ifndef NSPEEDTRACE
        auto t2 = clock();
        std::cout << "Sort completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
    }
    void TriggerManager::load()
    {
        auto& id_list = scen->body.Triggers.trigger_data;
        list_by_order = scen->body.Triggers.trigger_display_order_array;
        // // I have misunderstood that, so actually it is equivalent to "trigger_display_order_array"
        //list_by_order.resize(id_list.size());
        //for (TriggerStructIdx i = 0; i < order_list.size(); ++i)
        //{
        //    list_by_order[order_list[i]] = i;
        //}
    }
    void TriggerManager::confirm()
    {
        //TriggerManager class don't remove deleted trigger really, but perform this when confirm
        auto& id_list = scen->body.Triggers.trigger_data;
        if (!list_deleted.empty())
        {
            //sort to ensure the trigger which id is greater deleted first
            std::sort(list_deleted.begin(), list_deleted.end());
            //the trigger id which effect called should be repointed.
            for (size_t i = 0; i < list_by_order.size(); ++i)
            {
                trigger_repoint(id_list[i], list_deleted);
            }
            while (!list_deleted.empty())
            {
                for (auto& trig_idx : list_by_order)
                {
                    //decrease the index which greater than deleted
                    if (trig_idx > list_deleted.back())
                    {
                        --trig_idx;
                    }
                }
                id_list.erase(id_list.begin() + list_deleted.back());
                list_deleted.pop_back();
            }
        }
        //generate new trigger_display_order_array, and refresh triggers number data
        auto& order_list = scen->body.Triggers.trigger_display_order_array;
        order_list = list_by_order;
        scen->header.trigger_count = order_list.size();
        scen->body.Options.number_of_triggers = order_list.size();
        scen->body.Triggers.number_of_triggers = order_list.size();
    }
    void TriggerManager::trigger_repoint(TriggerStruct& trig, const vector<TriggerStructIdx>& list_deleted_sorted)
    {
        for (auto& effect : trig.effect_data)
        {
            if (effect.trigger_id != -1)
            {
                for (size_t i = 0; i <= list_deleted_sorted.size(); ++i)
                {
                    //decrease called id which greater than deleted
                    if (i == list_deleted_sorted.size())
                    {
                        effect.trigger_id -= list_deleted_sorted.size();
                        break;
                    }
                    if (effect.trigger_id < list_deleted_sorted[i])
                    {
                        effect.trigger_id -= i;
                        break;
                    }
                    //deleted id call shall repoint to -1
                    else if(effect.trigger_id == list_deleted_sorted[i])
                    {
                        effect.trigger_id = -1;
                        break;
                    }
                }
            }
        }
    }
    void TriggerManager::change_player(TriggerStruct& trig, int32_t base_player, int32_t player, uint32_t mode)
    {
		string player_mark(" (p1)");
		player_mark[3] = '0' + player;
		trig.trigger_name += player_mark;
		for (auto& condition : trig.condition_data)
		{
			if (mode & CP_MOD_SOURCE)
			{
				change_player_judge(base_player, player, mode, condition.source_player);
			}
			if (mode & CP_MOD_TARGET)
			{
				change_player_judge(base_player, player, mode, condition.target_player);
			}
		}
		for (auto& effect : trig.effect_data)
		{
			if (mode & CP_MOD_SOURCE)
			{
				change_player_judge(base_player, player, mode, effect.source_player);
			}
			if (mode & CP_MOD_TARGET)
			{
				change_player_judge(base_player, player, mode, effect.target_player);
			}
			if (mode & CP_MOD_COLOR)
			{
				change_player_judge(base_player, player, mode, effect.player_color);
			}
		}
	}
	void TriggerManager::change_player_judge(int32_t base_player, int32_t player, uint32_t mode, int32_t& current_attr)
	{
		if (current_attr != -1 && current_attr != 0
			&& !((mode & CP_STRICT_BASE) && current_attr != base_player))
		{
			current_attr = player;
		}
        /* not certainly effective */
		//else if ((mode & CP_ONE_TO_ALL) && current_attr == player)
		//{
		//	current_attr = base_player;
		//}
	}
    TriggerStructIdx TriggerManager::copy_to_all(TriggerStructIdx to_copy, uint32_t mode)
    {
        int32_t base_player = 1;
        TriggerStructIdx old_size = list_by_order.size();
        for (uint32_t i = 1; i < scen->header.player_count + 1; ++i)
        {
            if (i != base_player)
            {
                add(scen->body.Triggers.trigger_data[list_by_order[to_copy]]);
                change_player(scen->body.Triggers.trigger_data[list_by_order.back()], base_player, i, mode);
            }
        }
        mov(to_copy + 1, old_size, list_by_order.size());
        TriggerStructIdx added_cnt = list_by_order.size() - old_size;
        return added_cnt;
    }
    TriggerStructIdx TriggerManager::del_for_all(TriggerStructIdx to_del)
    {
        TriggerStructIdx deleted_cnt = scen->header.player_count - 1;
        del(to_del + 1, deleted_cnt);
        return deleted_cnt;
    }

    string TextIO::string_escape(const string& src)
    {
        string dst;
        dst.reserve(src.size());
        for (auto i = src.begin(); i != src.end(); ++i)
        {
            switch (*i)
            {
            case '\r': dst += "\\r"; break;
            case '\n': dst += "\\n"; break;
            case '\t': dst += "\\t"; break;
            case '\\': dst += "\\\\"; break;
            default: dst.push_back(*i); break;
            }
        }
        return dst;
    }
    string TextIO::string_unescape(const string& src)
    {
        string dst;
        dst.reserve(src.size());
        for (auto i = src.begin(); i != src.end(); ++i)
        {
            if (*i == '\\')
            {
                ++i;
                if (i != src.end())
                {
                    switch (*i)
                    {
                    case 'r': dst.push_back('\r'); break;
                    case 'n': dst.push_back('\n'); break;
                    case 't': dst.push_back('\t'); break;
                    case '\\': dst.push_back('\\'); break;
                    }
                }
            }
            else
            {
                dst.push_back(*i);
            }
        }
        return dst;
    }
    void TextIO::strout_check(const string& s, AutoFile& fout)
    {
        if (s.empty() == false && s[0] != '\0')
        {
            *fout << s << std::endl;
        }
    }
    void TextIO::print_strings(AoE2ScenarioCurrent& scen_content, AutoFile& fout)
    {
        strout_check(string_escape(scen_content.header.scenario_instructions), fout);
        strout_check(string_escape(scen_content.header.creator_name), fout);
        strout_check(string_escape(scen_content.body.DataHeader.filename), fout);
        strout_check(string_escape(scen_content.body.Messages.instructions), fout);
        strout_check(string_escape(scen_content.body.Messages.hints), fout);
        strout_check(string_escape(scen_content.body.Messages.victory), fout);
        strout_check(string_escape(scen_content.body.Messages.loss), fout);
        strout_check(string_escape(scen_content.body.Messages.history), fout);
        strout_check(string_escape(scen_content.body.Messages.scouts), fout);
        strout_check(string_escape(scen_content.body.Cinematics.PregameCinemaFilename), fout);
        strout_check(string_escape(scen_content.body.Cinematics.VictoryCinemaFilename), fout);
        strout_check(string_escape(scen_content.body.Cinematics.LossCinemaFilename), fout);
        strout_check(string_escape(scen_content.body.BackgroundImage.filename), fout);
        for (auto& str : scen_content.body.PlayerDataTwo.strings)
        {
            strout_check(string_escape(str), fout);
        }
        for (auto& ai_name : scen_content.body.PlayerDataTwo.ai_names)
        {
            strout_check(string_escape(ai_name), fout);
        }
        for (auto& ai_file : scen_content.body.PlayerDataTwo.ai_files)
        {
            strout_check(string_escape(ai_file.ai_per_file_text), fout);
        }
        strout_check(string_escape(scen_content.body.Map.water_definition), fout);
        strout_check(string_escape(scen_content.body.Map.map_color_mood), fout);
        strout_check(string_escape(scen_content.body.Map.script_name), fout);
        for (auto& player : scen_content.body.Units.player_data_3)
        {
            strout_check(string_escape(player.constant_name), fout);
        }
        for (auto& trigger : scen_content.body.Triggers.trigger_data)
        {
            strout_check(string_escape(trigger.trigger_description), fout);
            strout_check(string_escape(trigger.trigger_name), fout);
            strout_check(string_escape(trigger.short_description), fout);
            for (auto& effect : trigger.effect_data)
            {
                strout_check(string_escape(effect.message), fout);
                strout_check(string_escape(effect.sound_name), fout);
            }
            for (auto& condition : trigger.condition_data)
            {
                strout_check(string_escape(condition.xs_function), fout);
            }
        }
        for (auto& variable : scen_content.body.Triggers.variable_data)
        {
            strout_check(string_escape(variable.variable_name), fout);
        }
        strout_check(string_escape(scen_content.body.Files.script_file_path), fout);
        strout_check(string_escape(scen_content.body.Files.script_file_content), fout);
        for (auto& ai_file : scen_content.body.Files.ai_files)
        {
            strout_check(string_escape(ai_file.ai_file_name), fout);
            strout_check(string_escape(ai_file.ai_file), fout);
        }
    }

    string TextIO::getline_to_str(AutoFile& fin)
    {
        string s;
        getline(*fin, s);
        return s;
    }
    template <typename T> void TextIO::strin_check(vStr<T>& s, AutoFile& fin)
    {
        if (s.s.empty() == false && s.s[0] != '\0')
        {
            s = string_unescape(getline_to_str(fin));
        }
    }

    void TextIO::get_strings(AoE2ScenarioCurrent& scen_content, AutoFile& fin)
    {
        strin_check(scen_content.header.scenario_instructions, fin);
        strin_check(scen_content.header.creator_name, fin);
        strin_check(scen_content.body.DataHeader.filename, fin);
        strin_check(scen_content.body.Messages.instructions, fin);
        strin_check(scen_content.body.Messages.hints, fin);
        strin_check(scen_content.body.Messages.victory, fin);
        strin_check(scen_content.body.Messages.loss, fin);
        strin_check(scen_content.body.Messages.history, fin);
        strin_check(scen_content.body.Messages.scouts, fin);
        strin_check(scen_content.body.Cinematics.PregameCinemaFilename, fin);
        strin_check(scen_content.body.Cinematics.VictoryCinemaFilename, fin);
        strin_check(scen_content.body.Cinematics.LossCinemaFilename, fin);
        strin_check(scen_content.body.BackgroundImage.filename, fin);
        for (auto& str : scen_content.body.PlayerDataTwo.strings)
        {
            strin_check(str, fin);
        }
        for (auto& ai_name : scen_content.body.PlayerDataTwo.ai_names)
        {
            strin_check(ai_name, fin);
        }
        for (auto& ai_file : scen_content.body.PlayerDataTwo.ai_files)
        {
            strin_check(ai_file.ai_per_file_text, fin);
        }
        strin_check(scen_content.body.Map.water_definition, fin);
        strin_check(scen_content.body.Map.map_color_mood, fin);
        strin_check(scen_content.body.Map.script_name, fin);
        for (auto& player : scen_content.body.Units.player_data_3)
        {
            strin_check(player.constant_name, fin);
        }
        for (auto& trigger : scen_content.body.Triggers.trigger_data)
        {
            strin_check(trigger.trigger_description, fin);
            strin_check(trigger.trigger_name, fin);
            strin_check(trigger.short_description, fin);
            for (auto& effect : trigger.effect_data)
            {
                strin_check(effect.message, fin);
                strin_check(effect.sound_name, fin);
            }
            for (auto& condition : trigger.condition_data)
            {
                strin_check(condition.xs_function, fin);
            }
        }
        for (auto& variable : scen_content.body.Triggers.variable_data)
        {
            strin_check(variable.variable_name, fin);
        }
        strin_check(scen_content.body.Files.script_file_path, fin);
        strin_check(scen_content.body.Files.script_file_content, fin);
        for (auto& ai_file : scen_content.body.Files.ai_files)
        {
            strin_check(ai_file.ai_file_name, fin);
            strin_check(ai_file.ai_file, fin);
        }
    }
    void TextIO::fexport(const char* file_path)
    {
        AutoFile fout(file_path, ios::out);
        print_strings(*scen, fout);
    }
    void TextIO::fexport(const wchar_t* file_path)
    {
        AutoFile fout(file_path, ios::out);
        print_strings(*scen, fout);
    }
    void TextIO::fimport(const char* file_path)
    {
        AutoFile fin(file_path, ios::in);
        get_strings(*scen, fin);
    }
    void TextIO::fimport(const wchar_t* file_path)
    {
        AutoFile fin(file_path, ios::in);
        get_strings(*scen, fin);
    }
}
