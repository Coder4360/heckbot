#include "app_lib.cpp"

struct app_config;

struct app_config get_configuration(int argc, char** argv);
void bot_on_ready(dpp::cluster& bot, dpp::interaction_create_t);

void bot_on_interaction(dpp::cluster& bot, const dpp::interaction_create_t& event);
