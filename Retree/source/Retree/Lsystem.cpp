#include "pch.h"
#include "Retree/Lsystem.h"

using std::stringstream;
Lsystem::Lsystem(const string& axiom) : axiom(axiom) {}

void Lsystem::AddRule(char symbol, const string& rule) {
    rules[symbol] = rule;
}

void Lsystem::AddRule(const string& ruleStr) {
    char symbol = ruleStr[0];
    string rule = ruleStr.substr(2);
    rules[symbol] = rule;
}

void Lsystem::AddRules(const string& rules) {
    stringstream ss(rules);
    string rule;
    if (!rules.empty()) {
        while (std::getline(ss, rule, '\n')) {
            AddRule(rule);
        }
    }
}

void Lsystem::SetRules(const unordered_map<char, string>& newRules) {
    rules = newRules;
}

string Lsystem::gen(const string& str) {
    stringstream ss;
    for (const char& c : str) {
        if (rules.find(c) != rules.end()) {
            ss << rules[c];
        }
        else {
            ss << c;
        }
    }
    return ss.str();
}

string Lsystem::Generate(int iteration) {
    string current = axiom;
    for (int i = 0; i < iteration; i++) {
        current = gen(current);
    }
    //std::cout << "Lsystem Output :" << current<< std::endl;
    return current;
}

