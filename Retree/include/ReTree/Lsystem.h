#pragma once
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;
class Lsystem {
public:
    string axiom;
    unordered_map<char, string> rules;

    Lsystem(const string& axiom);

    void AddRule(char symbol, const string& rule);
    void AddRule(const  string& ruleStr);
    void AddRules(const string& rules);
    void SetRules(const unordered_map<char, string>& newRules);

private:
    string gen(const string& str);

public:
    string Generate(int iteration);
};

