#include <dpp/dpp.h>
#include "app_lib.hpp"
 
int main(int argc, char** argv) {
    // Load the config
    struct app_config config = get_configuration((unsigned int) argc, argv);

    dpp::cluster bot(config.token, dpp::intents::i_default_intents | dpp::intents::i_message_content);
 
    bot.on_log(dpp::utility::cout_logger()); // Add some logging to the terminal
 
    // On interaction event handler
    bot.on_interaction_create([&bot](const dpp::interaction_create_t& event) {
        bot_on_interaction((dpp::cluster&) bot, event);
    });

    // Bot on ready handler
    bot.on_ready([&bot, config](const dpp::ready_t& event) {
        bot_on_ready((dpp::cluster&) bot, event, config);
    });

    bot.on_message_create([&bot](const dpp::message_create_t& event) {
        bot_on_message_create((dpp::cluster&) bot, event);
    });
 
    // Start the bot
    bot.start(false);
}