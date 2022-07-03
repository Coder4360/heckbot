/**
 * @file app_lib.hpp
 * @author Coder436 (codergd@gmail.com)
 * @brief The app library
 * @version 0.1
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef BOT_LIB_INCLUDED
#define BOT_LIB_INCLUDED

struct app_config;

struct app_config get_configuration(int argc, char** argv);
void bot_on_ready(dpp::cluster& bot, dpp::interaction_create_t, struct app_config configuration);

void bot_on_interaction(dpp::cluster& bot, const dpp::interaction_create_t& event);

void bot_on_message_create(dpp::cluster& bot, const dpp::message_create_t& event);

#include "logging.cpp"
#include "polls.cpp"
#include "app_lib.cpp"
#endif