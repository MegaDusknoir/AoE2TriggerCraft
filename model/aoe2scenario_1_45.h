#pragma once
#include "../util/file_types.h"

namespace AoE2Scenario_1_45_Namespace
{
    using namespace AoE2ScenarioFileTypesNamespace;
    class AoE2Scenario_1_45
    {
    public:
        class FileHeader
        {
        public:
            size_t read(const char* raw_bin)
            {
                auto p_bin = raw_bin;
                data_read(version, p_bin);
                data_read(header_length, p_bin);
                data_read(savable, p_bin);
                data_read(timestamp_of_last_save, p_bin);
                scenario_instructions.read(p_bin);
                data_read(player_count, p_bin);
                data_read(unknown_value, p_bin);
                data_read(unknown_value_2, p_bin);
                data_read(amount_of_unknown_numbers, p_bin);
                vector_cst(unknown_numbers, p_bin, amount_of_unknown_numbers);
                creator_name.read(p_bin);
                data_read(trigger_count, p_bin);
                return p_bin - raw_bin;
            }
            size_t write(char* dst_bin) const
            {
                auto p_bin = dst_bin;
                data_write(version, p_bin);
                data_write(header_length, p_bin);
                data_write(savable, p_bin);
                data_write(timestamp_of_last_save, p_bin);
                data_write(scenario_instructions, p_bin);
                data_write(player_count, p_bin);
                data_write(unknown_value, p_bin);
                data_write(unknown_value_2, p_bin);
                data_write(amount_of_unknown_numbers, p_bin);
                data_write(unknown_numbers, p_bin);
                data_write(creator_name, p_bin);
                data_write(trigger_count, p_bin);
                return p_bin - dst_bin;
            }
        public:
            char version[4];
            uint32_t header_length;
            int32_t savable;
            uint32_t timestamp_of_last_save;
            str32 scenario_instructions;
            uint32_t player_count;
            uint32_t unknown_value; //Always (?) 1k
            uint32_t unknown_value_2; //Always (?) 1
            uint32_t amount_of_unknown_numbers;
            vector<uint32_t> unknown_numbers; // (!) may not be const 6
            str32 creator_name;
            uint32_t trigger_count;
        private:
        };
        class FileBody
        {
        public:
            size_t read(const char* raw_bin)
            {
                auto p_bin = raw_bin;
                DataHeader.read(p_bin);
                Messages.read(p_bin);
                Cinematics.read(p_bin);
                BackgroundImage.read(p_bin);
                PlayerDataTwo.read(p_bin);
                data_read(GlobalVictory, p_bin);
                Diplomacy.read(p_bin);
                Options.read(p_bin);
                Map.read(p_bin);
                Units.read(p_bin);
                Triggers.read(p_bin);
                Files.read(p_bin);
                return p_bin - raw_bin;
            }
            size_t write(char* dst_bin) const
            {
                auto p_bin = dst_bin;
                DataHeader.write(p_bin);
                Messages.write(p_bin);
                Cinematics.write(p_bin);
                BackgroundImage.write(p_bin);
                PlayerDataTwo.write(p_bin);
                data_write(GlobalVictory, p_bin);
                Diplomacy.write(p_bin);
                Options.write(p_bin);
                Map.write(p_bin);
                Units.write(p_bin);
                Triggers.write(p_bin);
                Files.write(p_bin);
                return p_bin - dst_bin;
            }
        public:
            /* Data Header */
            struct DataHeaderStruct
            {
                uint32_t next_unit_id_to_place;
                float version;
                char tribe_names[MAX_PLAYER_COUNT][256];
                uint32_t string_table_player_names[MAX_PLAYER_COUNT];
                struct PlayerDataOneStruct
                {
                    uint32_t active;
                    uint32_t human;
                    uint32_t civilization;
                    uint32_t architecture_set;
                    uint32_t cty_mode;
                } player_data_1[MAX_PLAYER_COUNT];
                uint32_t per_player_lock_civilization[MAX_PLAYER_COUNT];
                bytes<9> unknown;
                str16 filename;
                void read(const char*& p_bin)
                {
                    data_read(next_unit_id_to_place, p_bin);
                    data_read(version, p_bin);
                    data_read(tribe_names, p_bin);
                    data_read(string_table_player_names, p_bin);
                    data_read(player_data_1, p_bin);
                    data_read(per_player_lock_civilization, p_bin);
                    data_read(unknown, p_bin);
                    filename.read(p_bin);
                }
                void write(char*& p_bin) const
                {
                    data_write(next_unit_id_to_place, p_bin);
                    data_write(version, p_bin);
                    data_write(tribe_names, p_bin);
                    data_write(string_table_player_names, p_bin);
                    data_write(player_data_1, p_bin);
                    data_write(per_player_lock_civilization, p_bin);
                    data_write(unknown, p_bin);
                    data_write(filename, p_bin);
                }
            } DataHeader;
            /* Messages */
            struct MessagesStruct
            {
                uint32_t string_tables[6];
                str16 ascii_messages[6];
                void read(const char*& p_bin)
                {
                    data_read(string_tables, p_bin);
                    for (size_t i = 0; i < 6; ++i)
                    {
                        ascii_messages[i].read(p_bin);
                    }
                }
                void write(char*& p_bin) const
                {
                    data_write(string_tables, p_bin);
                    for (size_t i = 0; i < 6; ++i)
                    {
                        ascii_messages[i].write(p_bin);
                    }
                }
            } Messages;
            /* Cinematics */
            struct CinematicsStruct
            {
                str16 PregameCinemaFilename;
                str16 VictoryCinemaFilename;
                str16 LossCinemaFilename;
                void read(const char*& p_bin)
                {
                    data_read(PregameCinemaFilename, p_bin);
                    data_read(VictoryCinemaFilename, p_bin);
                    data_read(LossCinemaFilename, p_bin);
                }
                void write(char*& p_bin) const
                {
                    data_write(PregameCinemaFilename, p_bin);
                    data_write(VictoryCinemaFilename, p_bin);
                    data_write(LossCinemaFilename, p_bin);
                }
            } Cinematics;
            /* Background Image */
            struct BackgroundImageStruct
            {
                struct BitMapInfoStruct
                {
                    int32_t size;
                    uint32_t width;
                    uint32_t height;
                    int16_t planes;
                    int16_t bit_count;
                    uint32_t compression;
                    uint32_t image_size;
                    uint32_t x_pels;
                    uint32_t y_pels;
                    uint32_t number_of_colors_used;
                    uint32_t important_colors;
                    vector<uint32_t> colors_used;
                    vector<uint8_t> image;
                    void read(const char*& p_bin)
                    {
                        data_read(size, p_bin);
                        data_read(width, p_bin);
                        data_read(height, p_bin);
                        data_read(planes, p_bin);
                        data_read(bit_count, p_bin);
                        data_read(compression, p_bin);
                        data_read(image_size, p_bin);
                        data_read(x_pels, p_bin);
                        data_read(y_pels, p_bin);
                        data_read(number_of_colors_used, p_bin);
                        data_read(important_colors, p_bin);
                        vector_cst(colors_used, p_bin, number_of_colors_used);
                        vector_cst(image, p_bin, width * height);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(size, p_bin);
                        data_write(width, p_bin);
                        data_write(height, p_bin);
                        data_write(planes, p_bin);
                        data_write(bit_count, p_bin);
                        data_write(compression, p_bin);
                        data_write(image_size, p_bin);
                        data_write(x_pels, p_bin);
                        data_write(y_pels, p_bin);
                        data_write(number_of_colors_used, p_bin);
                        data_write(important_colors, p_bin);
                        data_write(colors_used, p_bin);
                        data_write(image, p_bin);
                    }
                };
                str16 filename;
                uint32_t picture_version;
                uint32_t bitmap_width;
                uint32_t bitmap_height;
                int16_t picture_orientation;
                vector<BitMapInfoStruct> bitmap_info;
                void read(const char*& p_bin)
                {
                    data_read(filename, p_bin);
                    data_read(picture_version, p_bin);
                    data_read(bitmap_width, p_bin);
                    data_read(bitmap_height, p_bin);
                    data_read(picture_orientation, p_bin);
                    vector_cst(bitmap_info, p_bin, bitmap_width * bitmap_height ? 1 : 0);
                }
                void write(char*& p_bin) const
                {
                    data_write(filename, p_bin);
                    data_write(picture_version, p_bin);
                    data_write(bitmap_width, p_bin);
                    data_write(bitmap_height, p_bin);
                    data_write(picture_orientation, p_bin);
                    data_write(bitmap_info, p_bin);
                }
            } BackgroundImage;
            /* PlayerDataTwo */
            struct PlayerDataTwoStruct
            {
                str16 strings[32];
                str16 ai_names[MAX_PLAYER_COUNT];
                struct AIStruct
                {
                    bytes<8> unknown;
                    str32 ai_per_file_text;
                    void read(const char*& p_bin)
                    {
                        data_read(unknown, p_bin);
                        data_read(ai_per_file_text, p_bin);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(unknown, p_bin);
                        data_write(ai_per_file_text, p_bin);
                    }
                } ai_files[MAX_PLAYER_COUNT];
                uint8_t ai_type[16];
                uint32_t separator;
                struct ResourcesStruct
                {
                    uint32_t gold;
                    uint32_t wood;
                    uint32_t food;
                    uint32_t stone;
                    uint32_t ore_x_unused;
                    uint32_t trade_goods;
                    uint32_t player_color;
                } resources[16];
                void read(const char*& p_bin)
                {
                    for (size_t i = 0; i < 32; ++i)
                    {
                        strings[i].read(p_bin);
                    }
                    for (size_t i = 0; i < MAX_PLAYER_COUNT; ++i)
                    {
                        ai_names[i].read(p_bin);
                    }
                    for (size_t i = 0; i < MAX_PLAYER_COUNT; ++i)
                    {
                        ai_files[i].read(p_bin);
                    }
                    data_read(ai_type, p_bin);
                    data_read(separator, p_bin);
                    data_read(resources, p_bin);
                }
                void write(char*& p_bin) const
                {
                    for (size_t i = 0; i < 32; ++i)
                    {
                        strings[i].write(p_bin);
                    }
                    for (size_t i = 0; i < MAX_PLAYER_COUNT; ++i)
                    {
                        ai_names[i].write(p_bin);
                    }
                    for (size_t i = 0; i < MAX_PLAYER_COUNT; ++i)
                    {
                        ai_files[i].write(p_bin);
                    }
                    data_write(ai_type, p_bin);
                    data_write(separator, p_bin);
                    data_write(resources, p_bin);
                }
            } PlayerDataTwo;
            /* Global Victory */
            struct GlobalVictoryStruct
            {
                uint32_t separator;
                uint32_t conquest_required;
                uint32_t ruins;
                uint32_t artifacts_required;
                uint32_t discovery;
                uint32_t explored_percent_of_map_required;
                uint32_t gold_required;
                uint32_t all_custom_conditions_required;
                uint32_t mode;
                uint32_t required_score_for_score_victory;
                uint32_t time_for_timed_game_in_10ths_of_a_year;
            } GlobalVictory;
            /* Diplomacy */
            struct DiplomacyStruct
            {
                struct PlayerDiplomacyStruct
                {
                    uint32_t stance_with_each_player[16];
                } per_player_diplomacy[16];
                bytes<60> individual_victories[192];
                uint32_t separator;
                uint32_t per_player_allied_victory[16];
                uint8_t lock_teams;
                uint8_t allow_players_choose_teams;
                uint8_t random_start_points;
                uint8_t max_number_of_teams;
                void read(const char*& p_bin)
                {
                    data_read(per_player_diplomacy, p_bin);
                    data_read(individual_victories, p_bin);
                    data_read(separator, p_bin);
                    data_read(per_player_allied_victory, p_bin);
                    data_read(lock_teams, p_bin);
                    data_read(allow_players_choose_teams, p_bin);
                    data_read(random_start_points, p_bin);
                    data_read(max_number_of_teams, p_bin);
                }
                void write(char*& p_bin) const
                {
                    data_write(per_player_diplomacy, p_bin);
                    data_write(individual_victories, p_bin);
                    data_write(separator, p_bin);
                    data_write(per_player_allied_victory, p_bin);
                    data_write(lock_teams, p_bin);
                    data_write(allow_players_choose_teams, p_bin);
                    data_write(random_start_points, p_bin);
                    data_write(max_number_of_teams, p_bin);
                }
            } Diplomacy;
            /* Options */
            struct OptionsStruct
            {
                uint32_t per_player_number_of_disabled_techs[16];
                vector<uint32_t> disabled_tech_ids_player[8];
                uint32_t per_player_number_of_disabled_units[16];
                vector<uint32_t> disabled_unit_ids_player[8];
                uint32_t per_player_number_of_disabled_buildings[16];
                vector<uint32_t> disabled_building_ids_player[8];
                uint32_t combat_mode;
                uint32_t naval_mode;
                uint32_t all_techs;
                uint32_t per_player_starting_age[16];
                bytes<17> unknown_1;
                uint8_t per_player_base_priority[8];
                bytes<7> unknown_2;
                uint32_t number_of_triggers;
                void read(const char*& p_bin)
                {
                    data_read(per_player_number_of_disabled_techs, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        vector_cst(disabled_tech_ids_player[i], p_bin, per_player_number_of_disabled_techs[i]);
                    }
                    data_read(per_player_number_of_disabled_units, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        vector_cst(disabled_unit_ids_player[i], p_bin, per_player_number_of_disabled_units[i]);
                    }
                    data_read(per_player_number_of_disabled_buildings, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        vector_cst(disabled_building_ids_player[i], p_bin, per_player_number_of_disabled_buildings[i]);
                    }
                    data_read(combat_mode, p_bin);
                    data_read(naval_mode, p_bin);
                    data_read(all_techs, p_bin);
                    data_read(per_player_starting_age, p_bin);
                    data_read(unknown_1, p_bin);
                    data_read(per_player_base_priority, p_bin);
                    data_read(unknown_2, p_bin);
                    data_read(number_of_triggers, p_bin);
                }
                void write(char*& p_bin) const
                {
                    data_write(per_player_number_of_disabled_techs, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        data_write(disabled_tech_ids_player[i], p_bin);
                    }
                    data_write(per_player_number_of_disabled_units, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        data_write(disabled_unit_ids_player[i], p_bin);
                    }
                    data_write(per_player_number_of_disabled_buildings, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        data_write(disabled_building_ids_player[i], p_bin);
                    }
                    data_write(combat_mode, p_bin);
                    data_write(naval_mode, p_bin);
                    data_write(all_techs, p_bin);
                    data_write(per_player_starting_age, p_bin);
                    data_write(unknown_1, p_bin);
                    data_write(per_player_base_priority, p_bin);
                    data_write(unknown_2, p_bin);
                    data_write(number_of_triggers, p_bin);
                }
            } Options;
            /* Map */
            struct MapStruct
            {
                bytes<2> separator_1;
                str16 water_definition;
                bytes<2> separator_2;
                str16 map_color_mood;
                bytes<2> separator_3;
                str16 script_name;
                uint8_t collide_and_correct;
                uint8_t villager_force_drop;
                bytes<128> unknown;
                uint8_t lock_coop_alliances;
                int32_t ai_map_type;
                uint32_t per_player_population_cap[16];
                uint32_t secondary_game_mode;
                bytes<4> unknown_3;
                bytes<4> unknown_4;
                //int32_t player_1_camera_y;
                //int32_t player_1_camera_x;
                int8_t no_waves_on_shore;
                int32_t map_width;
                int32_t map_height;
                struct TerrainStruct
                {
                    uint8_t terrain_id;
                    uint8_t elevation;
                    bytes<3> unused;
                    int16_t layer;
                    void read(const char*& p_bin)
                    {
                        data_read(terrain_id, p_bin);
                        data_read(elevation, p_bin);
                        data_read(unused, p_bin);
                        data_read(layer, p_bin);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(terrain_id, p_bin);
                        data_write(elevation, p_bin);
                        data_write(unused, p_bin);
                        data_write(layer, p_bin);
                    }
                };
                vector<TerrainStruct> terrain_data;
                void read(const char*& p_bin)
                {
                    data_read(separator_1, p_bin);
                    water_definition.read(p_bin);
                    data_read(separator_2, p_bin);
                    map_color_mood.read(p_bin);
                    data_read(separator_3, p_bin);
                    script_name.read(p_bin);
                    data_read(collide_and_correct, p_bin);
                    data_read(villager_force_drop, p_bin);
                    data_read(unknown, p_bin);
                    data_read(lock_coop_alliances, p_bin);
                    data_read(ai_map_type, p_bin);
                    data_read(per_player_population_cap, p_bin);
                    data_read(secondary_game_mode, p_bin);
                    data_read(unknown_3, p_bin);
                    data_read(unknown_4, p_bin);
                    data_read(no_waves_on_shore, p_bin);
                    data_read(map_width, p_bin);
                    data_read(map_height, p_bin);
                    vector_cst(terrain_data, p_bin, map_width * map_height);
                }
                void write(char*& p_bin) const
                {
                    data_write(separator_1, p_bin);
                    data_write(water_definition, p_bin);
                    data_write(separator_2, p_bin);
                    data_write(map_color_mood, p_bin);
                    data_write(separator_3, p_bin);
                    data_write(script_name, p_bin);
                    data_write(collide_and_correct, p_bin);
                    data_write(villager_force_drop, p_bin);
                    data_write(unknown, p_bin);
                    data_write(lock_coop_alliances, p_bin);
                    data_write(ai_map_type, p_bin);
                    data_write(per_player_population_cap, p_bin);
                    data_write(secondary_game_mode, p_bin);
                    data_write(unknown_3, p_bin);
                    data_write(unknown_4, p_bin);
                    data_write(no_waves_on_shore, p_bin);
                    data_write(map_width, p_bin);
                    data_write(map_height, p_bin);
                    data_write(terrain_data, p_bin);
                }
            } Map;
            /* Units */
            struct UnitsStruct
            {
                uint32_t number_of_unit_sections;
                struct PlayerDataFourStruct
                {
                    float food_duplicate;
                    float wood_duplicate;
                    float gold_duplicate;
                    float stone_duplicate;
                    float ore_x_duplicate;
                    float trade_goods_duplicate;
                    float population_limit;
                } player_data_4[8];
                uint32_t number_of_players;
                struct PlayerDataThreeStruct
                {
                    str16 constant_name;
                    float editor_camera_x;
                    float editor_camera_y;
                    int16_t initial_camera_x;
                    int16_t initial_camera_y;
                    uint8_t aok_allied_victory;
                    uint16_t player_count_for_diplomacy;
                    vector<uint8_t> diplomacy_for_interaction; //repeat:player_count_for_diplomacy
                    uint32_t diplomacy_for_ai_system[9];
                    uint32_t color;
                    float victory_version;
                    uint16_t unknown;
                    vector<uint8_t> unknown_2;//repeat:"7 if victory_version == 2 else 0"
                    vector<bytes<44>> unknown_structure_grand_theft_empires;//repeat:unknown
                    vector<uint8_t> unknown_5; //repeat:"1 if victory_version == 2 else 0"
                    uint8_t unknown_3[7];
                    vector<bytes<32>> unknown_structure_ww_campaign_2;//repeat: unknown_5
                    int32_t unknown_4;
                    void read(const char*& p_bin)
                    {
                        constant_name.read(p_bin);
                        data_read(editor_camera_x, p_bin);
                        data_read(editor_camera_y, p_bin);
                        data_read(initial_camera_x, p_bin);
                        data_read(initial_camera_y, p_bin);
                        data_read(aok_allied_victory, p_bin);
                        data_read(player_count_for_diplomacy, p_bin);
                        vector_cst(diplomacy_for_interaction, p_bin, player_count_for_diplomacy);
                        data_read(diplomacy_for_ai_system, p_bin);
                        data_read(color, p_bin);
                        data_read(victory_version, p_bin);
                        data_read(unknown, p_bin);
                        vector_cst(unknown_2, p_bin, victory_version == 2 ? 7 : 0);
                        vector_cst(unknown_structure_grand_theft_empires, p_bin, unknown);
                        vector_cst(unknown_5, p_bin, victory_version == 2 ? 1 : 0);
                        data_read(unknown_3, p_bin);
                        vector_cst(unknown_structure_ww_campaign_2, p_bin, victory_version == 2 ? unknown_5[0] : 0);
                        data_read(unknown_4, p_bin);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(constant_name, p_bin);
                        data_write(editor_camera_x, p_bin);
                        data_write(editor_camera_y, p_bin);
                        data_write(initial_camera_x, p_bin);
                        data_write(initial_camera_y, p_bin);
                        data_write(aok_allied_victory, p_bin);
                        data_write(player_count_for_diplomacy, p_bin);
                        data_write(diplomacy_for_interaction, p_bin);
                        data_write(diplomacy_for_ai_system, p_bin);
                        data_write(color, p_bin);
                        data_write(victory_version, p_bin);
                        data_write(unknown, p_bin);
                        data_write(unknown_2, p_bin);
                        data_write(unknown_structure_grand_theft_empires, p_bin);
                        data_write(unknown_5, p_bin);
                        data_write(unknown_3, p_bin);
                        data_write(unknown_structure_ww_campaign_2, p_bin);
                        data_write(unknown_4, p_bin);
                    }
                } player_data_3[8];
                struct PlayerUnitsStruct
                {
                    uint32_t unit_count;
                    struct UnitStruct
                    {
                        float x;
                        float y;
                        float z;
                        int32_t reference_id;
                        uint16_t unit_const;
                        uint8_t status;
                        float rotation;
                        uint16_t initial_animation_frame;
                        int32_t garrisoned_in_id;
                        void read(const char*& p_bin)
                        {
                            data_read(x, p_bin);
                            data_read(y, p_bin);
                            data_read(z, p_bin);
                            data_read(reference_id, p_bin);
                            data_read(unit_const, p_bin);
                            data_read(status, p_bin);
                            data_read(rotation, p_bin);
                            data_read(initial_animation_frame, p_bin);
                            data_read(garrisoned_in_id, p_bin);
                        }
                        void write(char*& p_bin) const
                        {
                            data_write(x, p_bin);
                            data_write(y, p_bin);
                            data_write(z, p_bin);
                            data_write(reference_id, p_bin);
                            data_write(unit_const, p_bin);
                            data_write(status, p_bin);
                            data_write(rotation, p_bin);
                            data_write(initial_animation_frame, p_bin);
                            data_write(garrisoned_in_id, p_bin);
                        }
                    };
                    vector<UnitStruct> units;//repeat: unit_count
                    void read(const char*& p_bin)
                    {
                        data_read(unit_count, p_bin);
                        vector_cst(units, p_bin, unit_count);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(unit_count, p_bin);
                        data_write(units, p_bin);
                    }
                };
                vector<PlayerUnitsStruct> players_units; // repeat: number_of_unit_sections
                void read(const char*& p_bin)
                {
                    data_read(number_of_unit_sections, p_bin);
                    data_read(player_data_4, p_bin);
                    data_read(number_of_players, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        player_data_3[i].read(p_bin);
                    }
                    vector_cst(players_units, p_bin, number_of_unit_sections);
                }
                void write(char*& p_bin) const
                {
                    data_write(number_of_unit_sections, p_bin);
                    data_write(player_data_4, p_bin);
                    data_write(number_of_players, p_bin);
                    for (size_t i = 0; i < 8; ++i)
                    {
                        player_data_3[i].write(p_bin);
                    }
                    data_write(players_units, p_bin);
                }
            } Units;
            /* Triggers */
            struct TriggersStruct
            {
                double trigger_version;
                int8_t trigger_instruction_start;
                int32_t number_of_triggers;
                struct TriggerStruct
                {
                    TriggerStruct()
                        :enabled(1),
                        looping(0),
                        description_string_table_id(0),
                        display_as_objective(0),
                        objective_description_order(0),
                        make_header(0),
                        short_description_string_table_id(0),
                        display_on_screen(0),
                        unknown(),
                        mute_objectives(0),
                        trigger_description(),
                        trigger_name(),
                        short_description(),
                        number_of_effects(0),
                        number_of_conditions(0)
                    {}
					uint32_t enabled;                           //开启
					int8_t looping;                             //循环
					int32_t description_string_table_id;        //触发描述字串ID
					uint8_t display_as_objective;               //显示任务
					uint32_t objective_description_order;       //显示任务顺序
					uint8_t make_header;                        //设为标题
                    int32_t short_description_string_table_id;  //简短描述字串ID
                    uint8_t display_on_screen;                  //在屏幕上显示
                    bytes<5> unknown;                           //unknown value regularly all 0
                    uint8_t mute_objectives;                    //静音任务
                    str32 trigger_description;                  //触发描述
                    str32 trigger_name;                         //触发名
                    str32 short_description;                    //简短描述
                    int32_t number_of_effects;
                    struct EffectStruct
                    {
                        int32_t effect_type;                        //效果类型
                        int32_t static_value_46;                    //Not used(?) regularly 48
                        int32_t ai_script_goal;                     //AI脚本goal
                        int32_t quantity;                           //数量
                        int32_t tribute_list;                       //资源列表
                        int32_t diplomacy;                          //外交
                        int32_t number_of_units_selected;           //已选择单位数量
                        int32_t legacy_location_object_reference;   //unknown regularly regularly -1
                        int32_t object_list_unit_id;                //单位列表ID
                        int32_t source_player;                      //源玩家
                        int32_t target_player;                      //目标玩家
                        int32_t technology;                         //科技
                        int32_t string_id;                          //字串ID
                        int32_t unknown_2;                          //
                        int32_t display_time;                       //定时器
                        int32_t trigger_id;                         //触发ID
                        int32_t location_x;
                        int32_t location_y;
                        int32_t area_x1;
                        int32_t area_y1;
                        int32_t area_x2;                            //shall >= x1
                        int32_t area_y2;                            //shall >= y1
                        int32_t object_group;                       //单位种属
                        int32_t object_type;                        //单位类型
                        int32_t instruction_panel_position;         //显示指南位置
                        int32_t attack_stance;                      //战斗姿态
                        int32_t time_unit;                          //定时器时间单位
                        int32_t enabled;                            //正向/逆向
                        int32_t food;                               //食物
                        int32_t wood;                               //木材
                        int32_t stone;                              //石料
                        int32_t gold;                               //黄金
                        int32_t item_id;                            //unknown regularly regularly -1
                        int32_t flash_object;                       //闪烁单位(不改权)
                        int32_t force_research_technology;          //强制研发科技
                        int32_t visibility_state;                   //视觉状态
                        int32_t scroll;                             //滚动(改变视角)
                        int32_t operation;                          //数值操作方式
                        int32_t object_list_unit_id_2;              //第二个单位列表ID
                        int32_t button_location;                    //按键位置
                        int32_t ai_signal_value;                    //AI信号值
                        int32_t unknown_3;                          //unknown regularly regularly -1
                        int32_t object_attributes;                  //单位属性
                        int32_t variable;                           //变量
                        int32_t timer;                              //定时器ID
                        int32_t facet;                              //物件朝向
                        int32_t location_object_reference;          //作为目标点的单位
                        int32_t play_sound;                         //指南播放声音
                        int32_t player_color;                       //玩家颜色
                        int32_t unknown_4;                          //unknown regularly regularly -1
                        int32_t color_mood;                         //色调
                        int32_t reset_timer;                        //重置定时器
                        int32_t object_state;                       //单位状态
                        int32_t action_type;                        //行动类型
                        str32 message;                              //信息
                        str32 sound_name;                           //声音名
                        vector<int32_t> selected_object_ids;        //选中单位列表
                        void read(const char*& p_bin)
                        {
                            data_read(effect_type, p_bin);
                            data_read(static_value_46, p_bin);
                            data_read(ai_script_goal, p_bin);
                            data_read(quantity, p_bin);
                            data_read(tribute_list, p_bin);
                            data_read(diplomacy, p_bin);
                            data_read(number_of_units_selected, p_bin);
                            data_read(legacy_location_object_reference, p_bin);
                            data_read(object_list_unit_id, p_bin);
                            data_read(source_player, p_bin);
                            data_read(target_player, p_bin);
                            data_read(technology, p_bin);
                            data_read(string_id, p_bin);
                            data_read(unknown_2, p_bin);
                            data_read(display_time, p_bin);
                            data_read(trigger_id, p_bin);
                            data_read(location_x, p_bin);
                            data_read(location_y, p_bin);
                            data_read(area_x1, p_bin);
                            data_read(area_y1, p_bin);
                            data_read(area_x2, p_bin);
                            data_read(area_y2, p_bin);
                            data_read(object_group, p_bin);
                            data_read(object_type, p_bin);
                            data_read(instruction_panel_position, p_bin);
                            data_read(attack_stance, p_bin);
                            data_read(time_unit, p_bin);
                            data_read(enabled, p_bin);
                            data_read(food, p_bin);
                            data_read(wood, p_bin);
                            data_read(stone, p_bin);
                            data_read(gold, p_bin);
                            data_read(item_id, p_bin);
                            data_read(flash_object, p_bin);
                            data_read(force_research_technology, p_bin);
                            data_read(visibility_state, p_bin);
                            data_read(scroll, p_bin);
                            data_read(operation, p_bin);
                            data_read(object_list_unit_id_2, p_bin);
                            data_read(button_location, p_bin);
                            data_read(ai_signal_value, p_bin);
                            data_read(unknown_3, p_bin);
                            data_read(object_attributes, p_bin);
                            data_read(variable, p_bin);
                            data_read(timer, p_bin);
                            data_read(facet, p_bin);
                            data_read(location_object_reference, p_bin);
                            data_read(play_sound, p_bin);
                            data_read(player_color, p_bin);
                            data_read(unknown_4, p_bin);
                            data_read(color_mood, p_bin);
                            data_read(reset_timer, p_bin);
                            data_read(object_state, p_bin);
                            data_read(action_type, p_bin);
                            data_read(message, p_bin);
                            data_read(sound_name, p_bin);
                            vector_cst(selected_object_ids, p_bin, number_of_units_selected > 0 ? number_of_units_selected : 0);
                        }
                        void write(char*& p_bin) const
                        {
                            data_write(effect_type, p_bin);
                            data_write(static_value_46, p_bin);
                            data_write(ai_script_goal, p_bin);
                            data_write(quantity, p_bin);
                            data_write(tribute_list, p_bin);
                            data_write(diplomacy, p_bin);
                            data_write(number_of_units_selected, p_bin);
                            data_write(legacy_location_object_reference, p_bin);
                            data_write(object_list_unit_id, p_bin);
                            data_write(source_player, p_bin);
                            data_write(target_player, p_bin);
                            data_write(technology, p_bin);
                            data_write(string_id, p_bin);
                            data_write(unknown_2, p_bin);
                            data_write(display_time, p_bin);
                            data_write(trigger_id, p_bin);
                            data_write(location_x, p_bin);
                            data_write(location_y, p_bin);
                            data_write(area_x1, p_bin);
                            data_write(area_y1, p_bin);
                            data_write(area_x2, p_bin);
                            data_write(area_y2, p_bin);
                            data_write(object_group, p_bin);
                            data_write(object_type, p_bin);
                            data_write(instruction_panel_position, p_bin);
                            data_write(attack_stance, p_bin);
                            data_write(time_unit, p_bin);
                            data_write(enabled, p_bin);
                            data_write(food, p_bin);
                            data_write(wood, p_bin);
                            data_write(stone, p_bin);
                            data_write(gold, p_bin);
                            data_write(item_id, p_bin);
                            data_write(flash_object, p_bin);
                            data_write(force_research_technology, p_bin);
                            data_write(visibility_state, p_bin);
                            data_write(scroll, p_bin);
                            data_write(operation, p_bin);
                            data_write(object_list_unit_id_2, p_bin);
                            data_write(button_location, p_bin);
                            data_write(ai_signal_value, p_bin);
                            data_write(unknown_3, p_bin);
                            data_write(object_attributes, p_bin);
                            data_write(variable, p_bin);
                            data_write(timer, p_bin);
                            data_write(facet, p_bin);
                            data_write(location_object_reference, p_bin);
                            data_write(play_sound, p_bin);
                            data_write(player_color, p_bin);
                            data_write(unknown_4, p_bin);
                            data_write(color_mood, p_bin);
                            data_write(reset_timer, p_bin);
                            data_write(object_state, p_bin);
                            data_write(action_type, p_bin);
                            data_write(message, p_bin);
                            data_write(sound_name, p_bin);
                            data_write(selected_object_ids, p_bin);
                        }
                    };
                    vector<EffectStruct> effect_data;           //re: number_of_effects
                    vector<int32_t> effect_display_order_array; //re: number_of_effects
                    int32_t number_of_conditions;
                    struct ConditionStruct
                    {
                        int32_t condition_type;     //条件类型
                        int32_t static_value_21;    //Not used(?) regularly 23
                        int32_t quantity;           //数量
                        int32_t attribute;          //属性
                        int32_t unit_object;        //设定单位
                        int32_t next_object;        //下一个单位
                        int32_t object_list;        //单位列表
                        int32_t source_player;      //源玩家
                        int32_t technology;         //科技
                        int32_t timer;              //定时器
                        int32_t unknown;            //unknown value regularly -1
                        int32_t area_x1;
                        int32_t area_y1;
                        int32_t area_x2;            //shall >= x1
                        int32_t area_y2;            //shall >= y1
                        int32_t object_group;       //单位种属
                        int32_t object_type;        //单位类型
                        int32_t ai_signal;          //AI信号
                        int32_t inverted;           //反转条件
                        int32_t unknown_2;          //unknown value regularly -1
                        int32_t variable;           //变量
                        int32_t comparison;         //比较条件
                        int32_t target_player;      //目标玩家
                        int32_t unit_ai_action;     //单位行动类型
                        int32_t unknown_4;          //unknown value regularly -1
                        int32_t object_state;       //单位状态
                        str32 xs_function;          //xs脚本
                        void read(const char*& p_bin)
                        {
                            data_read(condition_type, p_bin);
                            data_read(static_value_21, p_bin);
                            data_read(quantity, p_bin);
                            data_read(attribute, p_bin);
                            data_read(unit_object, p_bin);
                            data_read(next_object, p_bin);
                            data_read(object_list, p_bin);
                            data_read(source_player, p_bin);
                            data_read(technology, p_bin);
                            data_read(timer, p_bin);
                            data_read(unknown, p_bin);
                            data_read(area_x1, p_bin);
                            data_read(area_y1, p_bin);
                            data_read(area_x2, p_bin);
                            data_read(area_y2, p_bin);
                            data_read(object_group, p_bin);
                            data_read(object_type, p_bin);
                            data_read(ai_signal, p_bin);
                            data_read(inverted, p_bin);
                            data_read(unknown_2, p_bin);
                            data_read(variable, p_bin);
                            data_read(comparison, p_bin);
                            data_read(target_player, p_bin);
                            data_read(unit_ai_action, p_bin);
                            data_read(unknown_4, p_bin);
                            data_read(object_state, p_bin);
                            data_read(xs_function, p_bin);
                        }
                        void write(char*& p_bin) const
                        {
                            data_write(condition_type, p_bin);
                            data_write(static_value_21, p_bin);
                            data_write(quantity, p_bin);
                            data_write(attribute, p_bin);
                            data_write(unit_object, p_bin);
                            data_write(next_object, p_bin);
                            data_write(object_list, p_bin);
                            data_write(source_player, p_bin);
                            data_write(technology, p_bin);
                            data_write(timer, p_bin);
                            data_write(unknown, p_bin);
                            data_write(area_x1, p_bin);
                            data_write(area_y1, p_bin);
                            data_write(area_x2, p_bin);
                            data_write(area_y2, p_bin);
                            data_write(object_group, p_bin);
                            data_write(object_type, p_bin);
                            data_write(ai_signal, p_bin);
                            data_write(inverted, p_bin);
                            data_write(unknown_2, p_bin);
                            data_write(variable, p_bin);
                            data_write(comparison, p_bin);
                            data_write(target_player, p_bin);
                            data_write(unit_ai_action, p_bin);
                            data_write(unknown_4, p_bin);
                            data_write(object_state, p_bin);
                            data_write(xs_function, p_bin);
                        }
                    };
                    vector<ConditionStruct> condition_data;     //re: number_of_conditions
                    vector<int32_t> condition_display_order_array; //re: number_of_conditions
                    void read(const char*& p_bin)
                    {
                        data_read(enabled, p_bin);
                        data_read(looping, p_bin);
                        data_read(description_string_table_id, p_bin);
                        data_read(display_as_objective, p_bin);
                        data_read(objective_description_order, p_bin);
                        data_read(make_header, p_bin);
                        data_read(short_description_string_table_id, p_bin);
                        data_read(display_on_screen, p_bin);
                        data_read(unknown, p_bin);
                        data_read(mute_objectives, p_bin);
                        data_read(trigger_description, p_bin);
                        data_read(trigger_name, p_bin);
                        data_read(short_description, p_bin);
                        data_read(number_of_effects, p_bin);
                        vector_cst(effect_data, p_bin, number_of_effects);
                        vector_cst(effect_display_order_array, p_bin, number_of_effects);
                        data_read(number_of_conditions, p_bin);
                        vector_cst(condition_data, p_bin, number_of_conditions);
                        vector_cst(condition_display_order_array, p_bin, number_of_conditions);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(enabled, p_bin);
                        data_write(looping, p_bin);
                        data_write(description_string_table_id, p_bin);
                        data_write(display_as_objective, p_bin);
                        data_write(objective_description_order, p_bin);
                        data_write(make_header, p_bin);
                        data_write(short_description_string_table_id, p_bin);
                        data_write(display_on_screen, p_bin);
                        data_write(unknown, p_bin);
                        data_write(mute_objectives, p_bin);
                        data_write(trigger_description, p_bin);
                        data_write(trigger_name, p_bin);
                        data_write(short_description, p_bin);
                        data_write(number_of_effects, p_bin);
                        data_write(effect_data, p_bin);
                        data_write(effect_display_order_array, p_bin);
                        data_write(number_of_conditions, p_bin);
                        data_write(condition_data, p_bin);
                        data_write(condition_display_order_array, p_bin);
                    }
                };
                vector<TriggerStruct> trigger_data; //re: number_of_triggers
                vector<uint32_t> trigger_display_order_array; //re: number_of_triggers
                bytes<1028> unknown_bytes;
                uint32_t number_of_variables;
                struct VariableStruct
                {
                    uint32_t variable_id;
                    str32 variable_name;
                    void read(const char*& p_bin)
                    {
                        data_read(variable_id, p_bin);
                        data_read(variable_name, p_bin);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(variable_id, p_bin);
                        data_write(variable_name, p_bin);
                    }
                };
                vector<VariableStruct> variable_data; //re: number_of_variables
                void read(const char*& p_bin)
                {
                    data_read(trigger_version, p_bin);
                    data_read(trigger_instruction_start, p_bin);
                    data_read(number_of_triggers, p_bin);
                    vector_cst(trigger_data, p_bin, number_of_triggers);
                    vector_cst(trigger_display_order_array, p_bin, number_of_triggers);
                    data_read(unknown_bytes, p_bin);
                    data_read(number_of_variables, p_bin);
                    vector_cst(variable_data, p_bin, number_of_variables);
                }
                void write(char*& p_bin) const
                {
                    data_write(trigger_version, p_bin);
                    data_write(trigger_instruction_start, p_bin);
                    data_write(number_of_triggers, p_bin);
                    data_write(trigger_data, p_bin);
                    data_write(trigger_display_order_array, p_bin);
                    data_write(unknown_bytes, p_bin);
                    data_write(number_of_variables, p_bin);
                    data_write(variable_data, p_bin);
                }
            } Triggers;
            /* Files */
            struct FilesStruct
            {
                bytes<4> unknown_2;
                str16 script_file_path;
                str32 script_file_content;
                uint32_t ai_files_present;
                bytes<4> unknown_4;
                vector<uint32_t> number_of_ai_files; //rep: ai_files_present
                struct AI2Struct
                {
                    str32 ai_file_name;
                    str32 ai_file;
                    void read(const char*& p_bin)
                    {
                        data_read(ai_file_name, p_bin);
                        data_read(ai_file, p_bin);
                    }
                    void write(char*& p_bin) const
                    {
                        data_write(ai_file_name, p_bin);
                        data_write(ai_file, p_bin);
                    }
                };
                vector<AI2Struct> ai_files; //rep: if (number_of_ai_files.empty() != true) number_of_ai_files[0]
                void read(const char*& p_bin)
                {
                    data_read(unknown_2, p_bin);
                    data_read(script_file_path, p_bin);
                    data_read(script_file_content, p_bin);
                    data_read(ai_files_present, p_bin);
                    data_read(unknown_4, p_bin);
                    vector_cst(number_of_ai_files, p_bin, ai_files_present);
                    vector_cst(ai_files, p_bin, number_of_ai_files.empty() ? 0 : number_of_ai_files[0]);
                }
                void write(char*& p_bin) const
                {
                    data_write(unknown_2, p_bin);
                    data_write(script_file_path, p_bin);
                    data_write(script_file_content, p_bin);
                    data_write(ai_files_present, p_bin);
                    data_write(unknown_4, p_bin);
                    data_write(number_of_ai_files, p_bin);
                    data_write(ai_files, p_bin);
                }
            } Files;
        };
    public:
        FileHeader header;
        FileBody body;
    };
}
