#include <string>
#include <dpp/dpp.h>

struct app_config
{
    std::string token;
};

const std::string BOT_NAME = "Heckbot";

/**
 * @brief Get the configuration object
 * 
 * @param argc The argument count
 * @param argv The pointer to the arguments
 * @return struct app_config The bot configuration
 */
struct app_config get_configuration(int argc, char** argv) {
    if (argc < 1) {
        throw std::overflow_error("No argv 1, are you missing the bot token?");
    }
    return {
        token: argv[1]
    };
};

struct log_type {
    std::string name;
    uint32_t color;
};
namespace log_types {
    struct log_type LOG = {name: "Message", color: 0x808080};
    struct log_type INFO = {name: "Info", color: 0x0808EE};
    struct log_type WARNING = {name: "Warning", color: 0xEEEE08};
    struct log_type ERROR = {name: "Error", color: 0xEE0808};
    struct log_type CRITICAL = {name: "Critical Error", color: 0xB0000};
}

/**
 * @brief Get an embed for an error, warning, message...
 * 
 * @param type The messsage type
 * @param error_source The source of the error
 * @param fields A vector for the fields
 * @param timestamp The timestamp
 * @return dpp::embed The embed
 */
dpp::embed get_message_embed(struct log_type type, std::string error_source, std::vector<dpp::embed_field>
    fields, time_t timestamp) {
    dpp::embed em = dpp::embed()
        .set_title(BOT_NAME + ": " + type.name + " on " + error_source)
        .set_color(type.color)
        .set_timestamp(timestamp);
    for (dpp::embed_field field: fields) {
        em.add_field(field.name, field.value, field.is_inline);
    };
    return em;
}

/**
 * @brief Event handler for interactions, usually commands
 * 
 * @param bot The pointer to the bot
 * @param event The event object
 */
void bot_on_interaction(dpp::cluster& bot, const dpp::interaction_create_t& event) {
    std::string name = event.command.get_command_name();
    if (name == "ping") {
        // Ping pong command
        event.reply("Pong!");
    } else if (name == "rickroll") {
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
        long int count = std::get<long int>(event.get_parameter("count"));
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
    }
}

/**
 * @brief Event handler for when the bot is ready
 * 
 * @param bot A pointer to the bot
 * @param event The event (unused)
 */
void bot_on_ready(dpp::cluster& bot, const dpp::ready_t& event) {
    if (dpp::run_once<struct register_bot_commands>()) {
        bot.global_command_create(
            dpp::slashcommand("ping", "Ping pong!", bot.me.id)
        );
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
    }
}