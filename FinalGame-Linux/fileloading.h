#pragma once

#include <fstream>
#include <string>


char getFromFileChar(std::string filename, int valueNR){ //gets filename and order value (first, second)
    char value1, value2;
    std::ifstream values(filename);
    if (values){
        values >> value1 >> value2;
    }
    if(valueNR == 1){
        return value1;
    }
    else if(valueNR == 2){
        return value2;
    }
};

std::string getFromFileString(std::string filename, int valueNR){ //gets filename and order value (first, second)
    std::string value1, value2;
    std::ifstream values(filename);
    if (values){
        values >> value1 >> value2;
    }
    if(valueNR == 1){
        return value1;
    }
    else if(valueNR == 2){
        return value2;
    }
};

int getFromFileInt(std::string filename, int valueNR){ //gets filename and order value (first, second)
    int value1, value2;
    std::ifstream values(filename);
    if (values){
        values >> value1 >> value2;
    }
    if(valueNR == 1){
        return value1;
    }
    else if(valueNR == 2){
        return value2;
    }
};

int linesInFile(std::string filename){
    int number_of_lines = 0;
    std::string line;
    std::ifstream file(filename);
    while (std::getline(file, line)){
        ++number_of_lines;
    }
    if(number_of_lines < 1){ //to avoid trying to make a 0 element array if there is a file problem
        return 1;
    }
    else{
        return number_of_lines;
    }
};

