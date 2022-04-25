#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
#include <memory>
#include <algorithm>
#include "util/file_types.h"
#include "aoe2scenario_1_41.h"
#include "aoe2scenario_1_45.h"
#include "util/utilio.h"

#define OID_ADD_TRIGGER 0
#define OID_DEL_TRIGGER 1
#define OID_SORT_TRIGGER 2

#define CP_MOD_SOURCE 0x00000001L
#define CP_MOD_TARGET 0x00000002L
#define CP_MOD_COLOR  0x00000004L
#define CP_STRICT_BASE 0x00000008L
#define CP_ONE_TO_ALL 0x00000010L

namespace AoE2ScenarioNamespace
{
    using namespace AoE2ScenarioFileTypesNamespace;
    using namespace AoE2Scenario_1_41_Namespace;
    using namespace AoE2Scenario_1_45_Namespace;
    using AoE2ScenarioCurrent = AoE2Scenario_1_45;
    using struTrigger = AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct;
	using iterTrigger = vector<struTrigger>::iterator;
    using struTriggerOrder = uint32_t;
    using iterTriggerOrder = vector<struTriggerOrder>::iterator;

    class BaseOperator;
    class TriggerManager;

    class AoE2Scenario
	{
    private:
        void open(AutoFile& fin);
        void save(AutoFile& fout);
	public:
        AoE2Scenario();
		AoE2Scenario(const char* file_path);
        AoE2Scenario(const wchar_t* file_path);
        ~AoE2Scenario();
        void open(const wchar_t* file_path);
        void open(const char* file_path);
        void save(const wchar_t* file_path);
        void save(const char* file_path);
        AoE2ScenarioCurrent scen;
    private:
        deque<unique_ptr<BaseOperator>> operator_forward;
        deque<unique_ptr<BaseOperator>> operator_backward;
        static constexpr char current_version[] = "1.45";
        static DeflateClass deflate;
        string file_path;
        void deflate_decompress(string& raw);
        void deflate_compress(string& raw);
    public:
        unique_ptr<TriggerManager> triggers;
        void undo(void);
        void redo(void);
        void trigger_count_check(void)
        {
            auto trigger_count = scen.body.Triggers.trigger_data.size();
            scen.header.trigger_count = trigger_count;
            scen.body.Options.number_of_triggers = trigger_count;
            scen.body.Triggers.number_of_triggers = trigger_count;
        }
        iterTrigger add_trigger(int32_t location = -1);
        iterTrigger delete_trigger(int32_t front = -1, int32_t back = -1);
        void sort_trigger();
    };

    class TriggerManager
    {
        using TriggerStruct = AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct;
        using TriggerStructIdx = uint32_t;
        AoE2ScenarioCurrent* scen;
        vector<TriggerStructIdx> list_by_order;
        vector<TriggerStructIdx> list_deleted;
    public:
        TriggerManager(AoE2ScenarioCurrent* scn) :scen(scn)
        {}
        TriggerStruct& operator[](size_t idx);
        TriggerStruct& at(size_t idx);
        TriggerStructIdx size();
        TriggerStructIdx add();
        TriggerStructIdx add(const TriggerStruct& val);
        TriggerStructIdx add(TriggerStruct&& val);
        void del(TriggerStructIdx to_del);
        void del(TriggerStructIdx to_del_begin, TriggerStructIdx n);
        void mov(TriggerStructIdx target, TriggerStructIdx idx_begin, TriggerStructIdx idx_end);
        TriggerStructIdx copy_to_all(TriggerStructIdx to_copy, uint32_t mode);
        void sort_by_order();
        void load();
        void confirm();
    private:
        void trigger_repoint(TriggerStruct& trig, uint32_t old_id, int32_t new_id);
        void change_player(TriggerStruct& trig, int32_t base_player, int32_t player, uint32_t mode);
        void change_player_judge(int32_t base_player, int32_t player, uint32_t mode, int32_t& current_attr);
        void del(vector<TriggerStructIdx>::iterator to_del);
        void mov(vector<TriggerStructIdx>::iterator target, vector<TriggerStructIdx>::iterator idx_begin, vector<TriggerStructIdx>::iterator idx_end);
    };

    //deprecated redo/undo operators, may update in the future
    class BaseOperator
    {
    public:
        using operator_id_t = int;
        virtual void redo(void) = 0;
        virtual void undo(void) = 0;
        operator_id_t get_operator_id()
        {
            return operator_id;
        }
    private:
        static const operator_id_t operator_id;
    };

	class OperatorAddTrigger :public BaseOperator
	{
	public:
		OperatorAddTrigger(AoE2Scenario& scen, int32_t location = -1)
            :_scen(scen), _location(location)
		{
			redo();
		}
		void redo(void)
        {
			auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;
            if (_location < 0 || _location > trigger_data.size())
            {
                _location = trigger_data.size();
            }
            _return = trigger_data.insert(trigger_data.begin() + _location, AoE2ScenarioCurrent::FileBody::TriggersStruct::TriggerStruct());
            trigger_order.insert(trigger_order.begin() + _location, trigger_data.size() - 1);
            _scen.trigger_count_check();
        }
        void undo(void)
        {
            auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;
            trigger_data.erase(trigger_data.begin() + _location);
            trigger_order.erase(trigger_order.begin() + _location);
            _scen.trigger_count_check();
        }
        auto &get_return(void)
        {
            return _return;
        }
    private:
        static const int operator_id = OID_ADD_TRIGGER;
        AoE2Scenario& _scen;
        int32_t _location;
        iterTrigger _return;
    };

    class OperatorDelTrigger :public BaseOperator
    {
    public:
        OperatorDelTrigger(AoE2Scenario& scen, int32_t front = -1, int32_t back = -1)
            :_scen(scen), _front(front), _back(back)
        {
            redo();
        }
        void redo(void)
        {
            auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;
            if (_front < 0 || _front >= trigger_data.size())
            {
                _front = trigger_data.size() - 1;
            }
            if (_back < 0 || _back >= trigger_data.size())
            {
                _back = trigger_data.size() - 1;
            }
            if (_front > _back)
            {
                std::swap(_front, _back);
            }
            for (auto i = _front; i != _back + 1; ++i)
            {
                deleted_triggers.push_back(std::move(trigger_data[i]));
                deleted_triggers_order.push_back(std::move(trigger_order[i]));
            }
            _return = trigger_data.erase(trigger_data.begin() + _front, trigger_data.begin() + _back + 1);
            trigger_order.erase(trigger_order.begin() + _front, trigger_order.begin() + _back + 1);
            _scen.trigger_count_check();
        }
        void undo(void)
        {
            auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;
            trigger_data.insert(trigger_data.begin() + _front, deleted_triggers.begin(), deleted_triggers.end());
            trigger_order.insert(trigger_order.begin() + _front, deleted_triggers_order.begin(), deleted_triggers_order.end());
            _scen.trigger_count_check();
        }
        auto& get_return(void)
        {
            return _return;
        }
    private:
        static const int operator_id = OID_DEL_TRIGGER;
        AoE2Scenario& _scen;
        int32_t _front;
        int32_t _back;
        vector<struTrigger> deleted_triggers;
        vector<struTriggerOrder> deleted_triggers_order;
        iterTrigger _return;
    };

    class OperatorSortTrigger :public BaseOperator
    {
    public:
        OperatorSortTrigger(AoE2Scenario& scen)
            :_scen(scen), _return(nullptr)
        {
            redo();
        }
        void redo(void)
        {
            auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;
            //trigger_data.sort()

             //unfinished
        }
        void undo(void)
        {
            auto& trigger_data = _scen.scen.body.Triggers.trigger_data;
            auto& trigger_order = _scen.scen.body.Triggers.trigger_display_order_array;

            //unfinished
        }
        auto& get_return(void)
        {
            return _return;
        }
    private:
        static const int operator_id = OID_SORT_TRIGGER;
        AoE2Scenario& _scen;
        vector<struTriggerOrder> old_triggers_order;
        void *_return;
    };
}
