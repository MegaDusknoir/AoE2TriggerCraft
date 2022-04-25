
#include "aoe2scenario.h"

namespace AoE2ScenarioNamespace
{
    DeflateClass AoE2Scenario::deflate;
    AoE2Scenario::AoE2Scenario()
        :triggers(new TriggerManager(&this->scen))
    {
    }
    AoE2Scenario::~AoE2Scenario()
    {
    }
    AoE2Scenario::AoE2Scenario(const char* file_path)
        :triggers(new TriggerManager(&this->scen))
    {
        open(file_path);
    }
    AoE2Scenario::AoE2Scenario(const wchar_t* file_path)
        : triggers(new TriggerManager(&this->scen))
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
                throw std::runtime_error("Unexpected file length.");
            }
            triggers->load();
#ifndef NSPEEDTRACE
            auto t2 = clock();
            std::cout << "Read completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
        }
        else if (check_version == string("1.41"))
        {
            //Transform 1.41 to newest format.
        }
        else
        {
            string err_what;
            err_what = err_what + "Version " + check_version + " not supported yet.";
            throw std::runtime_error(err_what);
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
        old_id_list.resize(scen->body.Triggers.trigger_data.size());
        for (size_t i = 0; i < old_id_list.size(); ++i)
        {
            old_id_list[i] = i;
        }
        //swap trigger by display order
        for (size_t i = 0; i < list_by_order.size(); ++i)
        {
            if (list_by_order[i] != i)
            {
                std::swap(scen->body.Triggers.trigger_data[list_by_order[i]], scen->body.Triggers.trigger_data[i]);
                std::swap(old_id_list[list_by_order[i]], old_id_list[i]);
                list_by_order[i] = i;
            }
        }
        //repoint old id call to new
        for (TriggerStructIdx trig_idx = 0; trig_idx < list_by_order.size(); ++trig_idx)
        {
            for (size_t i = 0; i < old_id_list.size(); ++i)
            {
                if (old_id_list[i] != i)
                {
                    trigger_repoint(scen->body.Triggers.trigger_data[trig_idx], old_id_list[i], i);
                }
            }
        }
        //deleted trigger will be all swaped to the back, remap the delete list
        for (size_t i = 0; i < list_deleted.size(); ++i)
        {
            list_deleted[i] = list_by_order.size() + i;
        }
#ifndef NSPEEDTRACE
        auto t2 = clock();
        std::cout << "Sort completed in " << (double)(t2 - t1) / CLOCKS_PER_SEC << "sec" << std::endl;
#endif
    }
    void TriggerManager::load()
    {
        auto& id_list = scen->body.Triggers.trigger_data;
        auto& order_list = scen->body.Triggers.trigger_display_order_array;
        list_by_order.resize(id_list.size());
        for (TriggerStructIdx i = 0; i < order_list.size(); ++i)
        {
            list_by_order[order_list[i]] = i;
        }
    }
    void TriggerManager::confirm()
    {
        //TriggerManager class don't remove deleted trigger really, but perform this when confirm
        auto& id_list = scen->body.Triggers.trigger_data;
        if (!list_deleted.empty())
        {
            //sort to ensure the trigger which id is greater deleted first
            std::sort(list_deleted.begin(), list_deleted.end());
            while (!list_deleted.empty())
            {
                for (auto& trig_idx : list_by_order)
                {
                    //deleted id call shall repoint to -1
                    trigger_repoint(id_list[trig_idx], list_deleted.back(), -1);
                    //decrease the index which greater than deleted
                    if (trig_idx > list_deleted.back())
                    {
                        --trig_idx;
                    }
                }
                //decrease called id which greater than deleted
                for (auto i = list_deleted.back() + 1; i < list_by_order.size(); ++i)
                {
                    for (size_t j = 0; j < list_by_order.size(); ++j)
                    {
                        trigger_repoint(id_list[j], i, i - 1);
                    }
                }
                id_list.erase(id_list.begin() + list_deleted.back());
                list_deleted.pop_back();
            }
        }
        //generate new trigger_display_order_array, and refresh triggers number data
        auto& order_list = scen->body.Triggers.trigger_display_order_array;
        vector<uint32_t> new_order_list;
        new_order_list.resize(list_by_order.size());
        for (size_t i = 0; i < list_by_order.size(); ++i)
        {
            new_order_list[list_by_order[i]] = i;
        }
        order_list = std::move(new_order_list);
        scen->header.trigger_count = order_list.size();
        scen->body.Options.number_of_triggers = order_list.size();
        scen->body.Triggers.number_of_triggers = order_list.size();
    }
    void TriggerManager::trigger_repoint(TriggerStruct& trig, uint32_t old_id, int32_t new_id)
    {
        for (auto& effect : trig.effect_data)
        {
            if (effect.trigger_id == old_id)
            {
                effect.trigger_id = new_id;
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
}
