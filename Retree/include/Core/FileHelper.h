#pragma once
#include <string>
using std::string;

class FileHelper {
public:
    static string Open(const string& filter);
    static string Save(const string& filter);

    static string GetTimestampFilename();
    static string GetExtension(const string& filename);
    static string GetExtensionWithoutDot(const string& filename);
    static string GetFilenameWithoutExtension(const string& filename);

    static string NativeOpenTest();
   
};