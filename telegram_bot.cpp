#include <iostream>
#include <tgbot/tgbot.h>

int main() {
    std::string token = "6203688416:AAGhHafByVLPW2UzYuw4ACi0CpM0ITYxCyI";
    TgBot::Bot bot(token);

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        std::cout << "User wrote: " << message->text << std::endl;

        if (message->text.find("/start") == 0) {
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;

        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
