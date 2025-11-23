#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace FileReader {

    const char* read(const char* path) {

        std::ifstream file(path);

        std::string content;

        std::string line;

        if (!file.is_open()) {

            std::cout << "erro ao abrir o arquivo: " << path << std::endl;
            
            return nullptr;

        }
        
        while (std::getline(file, line)) 
            content += (line + '\n');

        file.close();

        char* buffer = new char[content.size() + 1];

        std::copy(content.begin(), content.end(), buffer);

        buffer[content.size()] = '\0';

        return buffer; 

    }

};