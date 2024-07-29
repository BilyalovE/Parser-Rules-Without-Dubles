#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <stdexcept>
#include <locale>
#include "tinyxml2.h"


using namespace tinyxml2;

int findLine(const std::string& inputFile, const std::string& elementName) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + inputFile);
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        if (line.find(elementName) != std::string::npos) {
            return lineNumber;
        }
    }
    return -1; // Element not found
}

void removeDuplicates(const std::string& inputFile, const std::string& outputFile) {
    XMLDocument doc;
    if (doc.LoadFile(inputFile.c_str()) != XML_SUCCESS) {
        throw std::runtime_error("Не удалось загрузить файл: " + inputFile);
    }

    int rootLine = findLine(inputFile, "<root>");
    XMLElement* root = doc.FirstChildElement("root");
    if (!root) {
        throw std::runtime_error("Не найден корневой элемент <root> на строке " + std::to_string(rootLine) + ".");
    }

    int rulesLine = findLine(inputFile, "<Rules>");
    XMLElement* rules = root->FirstChildElement("Rules");
    if (!rules) {
        throw std::runtime_error("Не найден элемент <Rules> на строке " + std::to_string(rulesLine) + ".");
    }

    std::set<std::pair<std::string, std::string>> ruleIdentifiers;
    std::vector<XMLElement*> uniqueRules;

    for (XMLElement* rule = rules->FirstChildElement("Rule"); rule != nullptr; rule = rule->NextSiblingElement("Rule")) {
        const char* tag = rule->Attribute("Tag");
        const char* type = rule->Attribute("Type");
        if (tag && type && ruleIdentifiers.insert({ type, tag }).second) {
            uniqueRules.push_back(static_cast<XMLElement*>(rule->DeepClone(&doc)));
        }
    }

    // Clear existing rules and add unique rules 
    while (XMLElement* rule = rules->FirstChildElement("Rule")) {
        rules->DeleteChild(rule);
    }

    for (XMLElement* rule : uniqueRules) {
        rules->InsertEndChild(rule);
    }

    if (doc.SaveFile(outputFile.c_str()) != XML_SUCCESS) {
        throw std::runtime_error("Не удалось сохранить файл: " + outputFile);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    try {
        std::string inputFile = "D:/Документы/Новая папка/ConsoleApplication1/RULES_CHTN_26.07.24_dubles.xml";
        std::string outputFile = "D:/Документы/Новая папка/ConsoleApplication1/RULES_CHTN_29.07.24.xml";
        removeDuplicates(inputFile, outputFile);
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}