//
//  main.cpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>

#include "json.hpp"
#include "writer.hpp"
#include "parser.hpp"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cout << "usage: smartjson input [output]" << std::endl;
        return 0;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile;
    if(argc > 2)
    {
        outputFile = argv[2];
    }
    
    if(outputFile.empty())
    {
        outputFile = inputFile + ".out";
    }
    
    std::ifstream fs(inputFile.c_str());
    if(fs.bad())
    {
        std::cout << "Failed to open input file:" << inputFile << std::endl;
        return 0;
    }
    
    size_t blockSize = 16 * 1024;
    size_t offset = 0;
    std::string buffer(blockSize, 0);
    while(true)
    {
        buffer.resize(offset + blockSize);
        fs.read(&buffer[offset], blockSize);
        size_t bytes = fs.gcount();
        
        offset += bytes;
        if(bytes < blockSize)
        {
            buffer.resize(offset);
            break;
        }
    }
    
    mjson::Parser parser;
    if(mjson::RC_OK != parser.parse(buffer.c_str(), buffer.size()))
    {
        std::cout << "Parse json Failed" << std::endl;
        return 0;
    }
    
    std::ofstream of(outputFile.c_str());
    mjson::Writer writer;
    writer.write(parser.getRoot(), of);
    of.close();
    return 0;
}