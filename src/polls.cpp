/**
 * @file polls.cpp
 * @brief Polls module for Deltabot
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef POLLS_CPP_INCLUDED
#define POLLS_CPP_INCLUDED
// Required headers
#include <dpp/dpp.h>
#include "app_lib.hpp"

/**
 * @brief A poll class
 *
 */
class Poll
{
public:
    Poll(dpp::snowflake author, std::string question)
    {
        this->author = author;
        this->question = question;
    };
    Poll()
    {
        author = 0;
    };
    void add_option(std::string option)
    {
        options.push_back(option);
    };
    unsigned long option_count()
    {
        return options.size();
    };
    ~Poll() {}
    dpp::snowflake author;
    std::string question;
    std::vector<std::string> options;
};

std::map<dpp::snowflake, Poll> current_user_polls;
const int MAX_POLL_OPTIONS = 20; // 20 because of the reaction limit
std::vector<std::string> emojis = {
    "\U0001f1e6", // :regional_indicator_a:
    "\U0001f1e7",
    "\U0001f1e8",
    "\U0001f1e9",
    "\U0001f1ea",
    "\U0001f1eb",
    "\U0001f1ec",
    "\U0001f1ed",
    "\U0001f1ee",
    "\U0001f1ef",
    "\U0001f1f0",
    "\U0001f1f1",
    "\U0001f1f2",
    "\U0001f1f3",
    "\U0001f1f4",
    "\U0001f1f5",
    "\U0001f1f6",
    "\U0001f1f7",
    "\U0001f1f8",
    "\U0001f1f9",
    "\U0001f1fa",
    "\U0001f1fb",
    "\U0001f1fc",
    "\U0001f1fd",
    "\U0001f1fe",
    "\U0001f1ff",
    ":one:",
    ":two:",
    ":three:",
    ":four:",
    ":five:",
    ":six:",
    ":seven:",
    ":eight:",
    ":nine:",
    ":zero:",
    ":white_circle:",
    ":black_circle:",
    ":red_circle:",
    ":blue_circle:",
    ":green_circle:",
    ":yellow_circle:",
    ":orange_circle:",
    ":white_large_square:",
    ":black_large_square:",
    ":red_large_square:",
    ":blue_large_square:",
    ":green_large_square:",
    ":yellow_large_square:",
    ":orange_large_square:"
};

/**
 * @brief Makes a poll
 *
 * @param event The command event
 */
void poll_make(dpp::interaction_create_t& event)
{
    // Get the author
    dpp::snowflake author = event.command.usr.id;
    // Get the question
    std::string question = std::get<std::string>(event.get_parameter("question"));

    // Create the poll
    Poll poll = Poll(author, question);

    // Check if the user is already making a poll
    if (current_user_polls.find(author) != current_user_polls.end())
    {
        event.reply(dpp::message(event.command.channel_id, get_message_embed(log_types::ERROR, "command /poll",
            {{
                name : "Error in command",
                value : "You are already making a poll, try posting the current one.",
                is_inline : false
            }},
            time(NULL))));
        return;
    }

    // Set the poll to the current user's poll
    current_user_polls.insert_or_assign(author, poll);
}

/**
 * @brief Adds an option to the current poll
 *
 * @param event The command event
 */
void poll_add_option(dpp::interaction_create_t &event)
{
    // Get the author
    dpp::snowflake author = event.command.usr.id;
    // Get the option
    std::string option = std::get<std::string>(event.get_parameter("option"));

    // Check if the user is making a poll
    if (current_user_polls.find(author) == current_user_polls.end())
    {
        event.reply(dpp::message(event.command.channel_id, get_message_embed(log_types::ERROR, "command /poll",
            {{
                name : "Error in command",
                value : "You are not making a poll, try creating one.",
                is_inline : false
            }},
            time(NULL))));
        return;
    }

    // Add the option to the poll
    current_user_polls[author].add_option(option);
}

struct Generated_Poll
{
    dpp::embed embed;
    bool successful;
    unsigned long answer_count;
};

/**
 * @brief Generates a poll embed from a poll and a user.
 * 
 * @param poll The poll
 * @param author The user to be shown with the embed
 * @return Generated_Poll The poll wrapped in a structure to avoid errors
 */
Generated_Poll poll_generate_embed(Poll poll, dpp::user author) {
    unsigned long option_count = poll.option_count();
    if (option_count > MAX_POLL_OPTIONS)
    {
        return {
            get_message_embed(log_types::ERROR, "command /poll",
                              {{
                                  name : "Error in command",
                                  value : "The poll has too many options",
                                  is_inline : false
                              }},
                              time(NULL)),
            false,
            0};
    }

    dpp::embed_author author_embed = {
        name : author.username,
        url : "https://discordapp.com/users/" + std::to_string(author.id),
        icon_url : author.get_avatar_url(128),
        proxy_icon_url : ""
    };

    // Create the embed
    dpp::embed em = dpp::embed()
                        .set_title(poll.question)
                        .set_author(author_embed)
                        .set_color(0x00FF00)
                        .set_footer(dpp::embed_footer()
                                        .set_text("Note: each count has one extra vote from the bot's automatic reaction"));

    // Add the options to the embed
    std::string question_content = "";
    for (unsigned int i = 0; i < poll.option_count(); i++)
    {
        question_content += emojis[i] + std::string(" ") + poll.options[i] + "\n";
    }
    em.add_field("Options (react to vote)", question_content);

    return Generated_Poll {em, true, poll.options.size()};
}

/**
 * @brief Generates a poll embed
 *
 * @param author The author of the poll
 * @return dpp::embed The poll embed
 */
Generated_Poll poll_geneare_embed_from_user(dpp::user author)
{
    // Check if the user is making a poll
    if (current_user_polls.find(author.id) == current_user_polls.end())
    {
        return Generated_Poll{
            get_message_embed(log_types::ERROR, "command /poll",
                              {{
                                  name : "Error in command",
                                  value : "You are not making a poll, try creating one.",
                                  is_inline : false
                              }},
                              time(NULL)),
            false,
            0};
    }

    // Generate the poll

    return poll_generate_embed(current_user_polls.at(author.id), author);
};

void poll_add_reactions(unsigned int count, dpp::message message, dpp::cluster &bot)
{
    // Add the reactions
    for (unsigned int i = 0; i < count; i++)
    {
        bot.message_add_reaction(message, emojis[i]);
    }
};
#endif