#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

int loadScore() {
    std::ifstream infile;
    infile.open("scores.txt");
    std::string data;
    infile >> data;
    //Check if first row in file is empty
    if (data == "") {
        std::cout << "No data found." << std::endl;
        return 0;
    }

    infile.close();
    return stoi(data);
}

void saveScore(int score) {
    int prevScore = loadScore();
    std::ofstream outfile;
    outfile.open("scores.txt");

    //Save score to file
    if(score > prevScore || prevScore == 0){
        outfile << score;
    }
    else{
        outfile << prevScore;
    }
    outfile.close();
}