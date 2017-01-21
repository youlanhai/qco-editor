//
//  writer.hpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#ifndef writer_hpp
#define writer_hpp

#include "config.hpp"
#include <iostream>

namespace mjson
{
    class Node;
    
    class SMARTJSON_DLL Writer
    {
    public:
        Writer(const char *tab = "\t", const char *eol = "\n");
        
        void write(const Node &node, std::ostream &out);
        
        void writeNull(const Node &node, std::ostream &out);
        void writeBool(const Node &node, std::ostream &out);
        void writeInt(const Node &node, std::ostream &out);
        void writeFloat(const Node &node, std::ostream &out);
        void writeString(const Node &node, std::ostream &out);
        void writeNode(const Node &node, std::ostream &out, int depth);
        void writeArray(const Node &node, std::ostream &out, int depth);
        void writeDict(const Node &node, std::ostream &out, int depth);
        
    private:
        const char*     tab_;
        const char*     eol_;
    };
}

#endif /* writer_hpp */
