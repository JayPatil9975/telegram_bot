#include <iostream>
#include <fstream>
#include <tgbot/tgbot.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;

std::string convertImageToPdf(const std::string& imagePath, const std::string& pdfPath) {
    Mat image = imread(imagePath, IMREAD_UNCHANGED);
    if (image.empty()) {
        return "Failed to read the image.";
    }

    try {
        cv::imwrite(pdfPath, image);
    } catch (const cv::Exception& e) {
        return "Failed to convert image to PDF.";
    }

    return "Image successfully converted to PDF.";
}

int main() {
    std::string token = "6275349961:AAEGjMYxtmxvELGnIYmzCInhiS2yK2_fG28";
    TgBot::Bot bot(token);

    bot.getEvents().onCommand("convert", [&bot](TgBot::Message::Ptr message) {
        if (message->document) {
            std::string fileId = message->document->fileId;
            std::string filePath = bot.getApi().getFilePath(fileId);

            std::string imageLocalPath = "image.jpg";
            std::string pdfLocalPath = "converted.pdf";

            std::ofstream imageFile(imageLocalPath, std::ios::binary);
            bot.getApi().downloadFile(filePath, imageFile);
            imageFile.close();

            std::string result = convertImageToPdf(imageLocalPath, pdfLocalPath);

            std::ifstream pdfFile(pdfLocalPath, std::ios::binary);
            bot.getApi().sendDocument(message->chat->id, pdfFile);
            pdfFile.close();

            bot.getApi().sendMessage(message->chat->id, result);

            // Clean up the temporary files
            std::remove(imageLocalPath.c_str());
            std::remove(pdfLocalPath.c_str());
        } else {
            bot.getApi().sendMessage(message->chat->id, "Please upload an image file.");
        }
    });

    try {
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
