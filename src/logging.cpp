#include <dpp/dpp.h>

/** @brief The bot name */
const std::string BOT_NAME = "Deltabot";

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