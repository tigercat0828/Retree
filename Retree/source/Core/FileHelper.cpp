#include "pch.h"
#include "Core/FileHelper.h"
#include <nativefiledialog/nfd.h>

using std::string;
namespace  fs = std::filesystem;
std::string FileHelper::GetTimestampFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;

#ifdef _WIN32
    localtime_s(&localTime, &timeNow);  // Windows-safe
#else
    localtime_r(&timeNow, &localTime);  // POSIX-safe
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

string FileHelper::Open(const string& filter) {
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(filter.c_str(), NULL, &outPath);

    if (result == NFD_OKAY) {
        string filepath = outPath;
        free(outPath);
        return filepath;
    }
    else if (result == NFD_CANCEL) {
        return "";
    }
    else {
        printf("Error: %s\n", NFD_GetError());
        return "";
    }
}

string FileHelper::Save(const string& filter) {
    nfdchar_t* savePath = NULL;
    nfdresult_t result = NFD_SaveDialog(filter.c_str(), NULL, &savePath);
    if (result == NFD_OKAY) {
        string filepath = savePath;
        free(savePath);
        return filepath;
    }
    else if (result == NFD_CANCEL) {
        return "";
    }
    else {
        printf("Error: %s\n", NFD_GetError());
        return "";
    }
}

std::string FileHelper::GetExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string extWithDot = path.extension().string();
    return extWithDot;
}

std::string FileHelper::GetExtensionWithoutDot(const std::string& filename) {
    std::filesystem::path path(filename);
    std::string extWithDot = path.extension().string();
    if (!extWithDot.empty() && extWithDot[0] == '.') {
        extWithDot.erase(0, 1);
    }
    return extWithDot;
}

std::string FileHelper::GetFilenameWithoutExtension(const std::string& filename) {
    std::filesystem::path path(filename);
    return  path.stem().string();
}

string FileHelper::NativeOpenTest() {
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

    if (result == NFD_OKAY) {
        puts("Success!");
        puts(outPath);
        free(outPath);
    }
    else if (result == NFD_CANCEL) {
        puts("User pressed cancel.");
    }
    else {
        printf("Error: %s\n", NFD_GetError());
    }
    return "";
}

