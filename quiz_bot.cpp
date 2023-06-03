#include <iostream>
#include <tgbot/tgbot.h>
#include <vector>
#include <string>

struct QuizQuestion {
    std::string question;
    std::vector<std::string> options;
    int correctOptionIndex;

    bool operator==(const QuizQuestion& other) const {
        return question == other.question &&
               options == other.options &&
               correctOptionIndex == other.correctOptionIndex;
    }
};

std::vector<QuizQuestion> quizQuestions = {
    {
        "What is the capital of France?",
        {"London", "Paris", "Berlin", "Rome"},
        1 // Correct option index
    },
    {
        "Which planet is known as the Red Planet?",
        {"Venus", "Jupiter", "Mars", "Saturn"},
        2 // Correct option index
    }
};

void sendQuizQuestion(TgBot::Bot& bot, const TgBot::Message::Ptr& message, const QuizQuestion& question) {
    std::string questionText = question.question + "\n\n";
    for (size_t i = 0; i < question.options.size(); ++i) {
        questionText += std::to_string(i + 1) + ". " + question.options[i] + "\n";
    }
    bot.getApi().sendMessage(message->chat->id, questionText);
}

void handleUserAnswer(TgBot::Bot& bot, const TgBot::Message::Ptr& message, const QuizQuestion& question, int userAnswer) {
    if (userAnswer == question.correctOptionIndex) {
        // Correct answer
        bot.getApi().sendMessage(message->chat->id, "Correct answer!");
    } else {
        // Incorrect answer
        bot.getApi().sendMessage(message->chat->id, "Incorrect answer!");
    }

    // Send the next question or end the quiz based on the progress
    // Here, we simply send the next question from the quizQuestions vector
    size_t currentQuestionIndex = std::distance(quizQuestions.begin(), std::find(quizQuestions.begin(), quizQuestions.end(), question));
    if (currentQuestionIndex < quizQuestions.size() - 1) {
        const QuizQuestion& nextQuestion = quizQuestions[currentQuestionIndex + 1];
        sendQuizQuestion(bot, message, nextQuestion);
    } else {
        bot.getApi().sendMessage(message->chat->id, "Quiz completed!");
    }
}

int main() {
    TgBot::Bot bot("<015345198:AAFkwf73EvqEkyt3p5_RdRwjrqFjStY8nV0:>");

    bot.getEvents().onCommand("startquiz", [&bot](TgBot::Message::Ptr message) {
        // Send the first question
        const QuizQuestion& firstQuestion = quizQuestions.front();
        sendQuizQuestion(bot, message, firstQuestion);
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        std::string userAnswer = message->text;
        int optionIndex = std::stoi(userAnswer) - 1;

        // Retrieve the current question
        const QuizQuestion& currentQuestion = quizQuestions.front();

        // Handle the user's answer
        handleUserAnswer(bot, message, currentQuestion, optionIndex);

        // Remove the current question from the quizQuestions vector
        quizQuestions.erase(quizQuestions.begin());
    });

    bot.getApi().deleteWebhook();
    TgBot::TgLongPoll longPoll(bot);

    while (true) {
        try {
            longPoll.start();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
