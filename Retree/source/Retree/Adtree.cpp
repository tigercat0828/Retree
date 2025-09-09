#include "pch.h"
#include "Retree/Adtree.h"
#include <filesystem>

namespace fs = std::filesystem;
string Adtree::Run(const string& input) {
    string command = std::format("AdTree\\AdTree {} temp -s", input);
    Logger::Info("AdTree command launched : {}", command);
    int code = system(command.c_str());
    fs::path filePath = input;
    string filename = filePath.stem().string();
    string resultPLY = filename + "_skeleton.ply";
    string resultOBJ = filename + "_branches.obj";
    return fs::absolute("temp").string();
}
int Adtree::Test() {
    string command = "AdTree\\AdTree AdTree\\tree1.xyz AdTree\\output -s";
    int code = system(command.c_str());
    return code;
}


