/**
 * @file app_lib.cpp
 * @brief The code for the app_lib.hpp file
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <string>
#include <dpp/dpp.h>
#include <fstream>

struct app_config
{
    std::string token;
    bool register_commands;
};

/**
 * @brief Get the configuration object
 * 
 * @param argc The argument count
 * @param argv The pointer to the arguments
 * @return The bot configuration
 */
struct app_config get_configuration(unsigned int argc, char** argv) {
    std::ifstream token_file("token.txt");
    std::string token;
    std::getline(token_file, token);

    bool register_commands = false;
    if (argc >= 1) {
        for (unsigned int i = 0; i < argc; i++) {
            if (strcmp(argv[i], "--register-commands") == 0) {
                register_commands = true;
            }
        }
    }

    return {
        token: token,
        register_commands: register_commands
    };
};

/**
 * @brief Event handler for interactions, usually commands
 * 
 * @param bot The pointer to the bot
 * @param event The event object
 */
void bot_on_interaction(dpp::cluster& bot, const dpp::interaction_create_t& event) {
    if (event.command.type == dpp::interaction_type::it_application_command) {
        std::string name = event.command.get_command_name();
        if (name == "rickroll") {
            // Rickroll command
            dpp::embed em = dpp::embed()
                .set_title("Never gonna give you up")
                .set_color(0xAF13CF)
                .set_image("https://imagenes.20minutos.es/uploads/imagenes/2020/06/19/rick-astley-en-su-cancion-never-gonna-give-you-up.gif")
                .set_url("https://www.youtube.com/watch?v=dQw4w9WgXcQ")
                .add_field("Rickroll", "Never gonna give you up\nNever gonna let you down\nNever gonna turn around and desrt you")
                .set_footer(
                    dpp::embed_footer()
                        .set_text("Never gonna give you up, never gonna let you down")
                );
            event.reply(dpp::message(event.command.channel_id, em));
        } else if (name == "coinflip") {
            if ((std::rand() & 1) == 0) {
                event.reply("The coin landed heads.");
            } else {
                event.reply("The coin landed tails.");
            }
        } else if (name == "delete") {
            dpp::channel chn = bot.channel_get_sync(event.command.channel_id);
            int64_t count = std::get<int64_t>(event.get_parameter("count"));
            if (count <= 0) {
                event.reply(dpp::message(event.command.channel_id, get_message_embed(log_types::ERROR, "command /delete",
                {
                    {
                        name: "Error in parameter <count>",
                        value: "Count must be larger than 1",
                        is_inline: false
                    }
                }, time(NULL))));
            }
            dpp::message_map msg_map = bot.messages_get_sync(chn.id, chn.last_message_id, 0, 0, 2*count);
            std::vector<dpp::snowflake> msgs = {};
            for (std::pair<dpp::snowflake, dpp::message> pair: msg_map) {
                msgs.push_back(pair.first);
                if (pair.second.sent - time(NULL) >= 12098800) {
                    event.reply(dpp::message(event.command.channel_id, get_message_embed(log_types::ERROR, "command /delete",
                    {
                        {
                            name: "Time error",
                            value: "Cannot delete messages sent 14 or more days ago.",
                            is_inline: false
                        }
                    }, time(NULL)
                    )));
                    return;
                }
            }
            dpp::confirmation confirmation = bot.message_delete_bulk_sync(msgs, chn.id);
            if (confirmation.success) {
                event.reply("Successfully deleted " + std::to_string(count) + " messages.");
            } else {
                event.reply(dpp::message(event.command.channel_id, get_message_embed(log_types::ERROR, "command /delete", 
                {
                    {
                        name: "Permission error",
                        value: "Could not delete " + std::to_string(count) + " messages because of permissions",
                        is_inline: false
                    }
                }, time(NULL))));
            }
        } else if (name == "poll") {
            poll_make((dpp::interaction_create_t&) event);
        } else if (name == "poll_add") {
            poll_add_option((dpp::interaction_create_t&) event);
        } else if (name == "poll_send") {
            Generated_Poll poll = poll_geneare_embed_from_user(event.command.usr);
            dpp::message msg = dpp::message(event.command.channel_id, poll.embed);
            msg.set_reference(event.command.message_id);
            msg = bot.message_create_sync(msg);
            current_user_polls.erase(event.command.usr.id);
            for (unsigned int i = 0; i < poll.answer_count; i++) {
                bot.message_add_reaction(msg, emojis[i]);
            }
        }
    }
}

/**
 * @brief Event handler for when the bot is ready
 * 
 * @param bot A pointer to the bot
 * @param event The event (unused)
 */
void bot_on_ready(dpp::cluster& bot, const dpp::ready_t& event, struct app_config configuration) {
    if (configuration.register_commands || dpp::run_once<struct register_commands>()) {
        bot.global_command_create(
            dpp::slashcommand("rickroll", "Never gonna give you up.", bot.me.id)
        );
        bot.global_command_create(
            dpp::slashcommand("coinflip", "Flips a coin", bot.me.id)
        );
        bot.global_command_create(
            dpp::slashcommand("delete", "Bulk deletes messages", bot.me.id)
                .add_option(dpp::command_option(dpp::co_integer, "count", "The amount"))
        );
        bot.global_command_create(
            dpp::slashcommand("poll", "Creates a poll", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "question", "The question"))
        );
        bot.global_command_create(
            dpp::slashcommand("poll_add", "Adds an option to a poll", bot.me.id)
                .add_option(dpp::command_option(dpp::co_string, "option", "The option"))
        );
        bot.global_command_create(
            dpp::slashcommand("poll_send", "Sends a poll", bot.me.id)
        );
    }
}


/**
 * @brief Event handler for when a message is received
 * 
 * @param bot A pointer to the bot (unused)
 * @param event The event
 */
void bot_on_message_create(dpp::cluster& bot, const dpp::message_create_t& event) {
    if (event.msg.author.is_bot()) { return; }
    std::string content = event.msg.content;
    event.reply(content);
}