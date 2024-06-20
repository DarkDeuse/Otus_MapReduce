#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

#include "libs/MapReduce.h"

void mapFunction (std::ifstream& fileIn, 
                MapReduce::pos_t& begin, 
                MapReduce::pos_t& end  , 
                MapReduce::vString_t& vectStr,
                std::mutex& mtx)
{
    // read raw bytes from file
    std::vector<char> buffer(static_cast<size_t>(end-begin));
    {
        std::lock_guard<std::mutex> lock(mtx);
        fileIn.seekg(begin, std::ios_base::beg);
        fileIn.read(buffer.data(), buffer.size());
    }

    // convert to strings
    
    //char *pred;
    size_t i{0};
    (void)i;
    for (auto iter = strtok(buffer.data(),"\n"); iter != nullptr; iter = strtok(nullptr,"\n")){
        std::string str1 {iter};
        vectStr.emplace_back(iter);
        ++i;
    } 
        
}

void reduceFunction(MapReduce::vString_t& vectorIn, size_t& taskNumber)
{
    
    size_t answer{0}, buf, sizeStr, sizePred{0};
    std::string pred {"0"};
    for (auto& str : vectorIn)
    {
        buf = 0;
        sizeStr = str.size();

        while(buf<std::min(sizeStr, sizePred) && str[buf] == pred[buf]) 
             ++buf;

        if (buf>answer)
            answer = buf;

        pred = str;
        sizePred = sizeStr;
    }
    std::ofstream out;
    out.open("reducer_" + std::to_string(taskNumber) +  "_.log");
    out <<  "minimum prefix size = " << answer;
    out.close();
}


int main(/*int argc, char** argv*/){

    /*if (argc != 4)
    {
        std::cout << "Wrong arguments. Usage: map_reduce <src> <mnum> <rnum>" << std::endl;
        //return -1;
    };

    size_t countMaps = std::stoi(argv[2]);
    if (countMaps <= 0)
    {
        std::cout << "mnum should be > 0" << std::endl;
       // return -2;
    }

    size_t countReducers = std::stoi(argv[3]);
    if (countReducers <= 0)
    {
        std::cout << "rnum should be > 0" << std::endl;
        //return -3;
    }*/

    std::string s1 {"test1"};
    size_t ms = 5;
    size_t rs = 5;
    
    MapReduce mp1{s1, ms, rs};

    mp1.setMapper(mapFunction);
    mp1.setReducer(reduceFunction);

    mp1.start();

    return 0;
}
