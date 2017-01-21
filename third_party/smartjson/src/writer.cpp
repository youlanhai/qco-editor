//
//  writter.cpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#include "writer.hpp"
#include "node.hpp"
#include "container/dict.hpp"

namespace mjson
{
    struct Tab
    {
        int             n_;
        const char*     tab_;
        
        Tab(int n, const char *tab)
        : n_(n)
        , tab_(tab)
        {}
        
        friend std::ostream& operator << (std::ostream& out, const Tab &tab)
        {
            for(int i = 0; i < tab.n_; ++i)
            {
                out << tab.tab_;
            }
            return out;
        }
    };
    
    Writer::Writer(const char *tab, const char *eol)
    : tab_(tab)
    , eol_(eol)
    {
        
    }
    
    void Writer::write(const Node &node, std::ostream &out)
    {
        writeNode(node, out, 0);
        out << eol_;
    }
    
    void Writer::writeNode(const Node &node, std::ostream &out, int depth)
    {
        if(node.isNull())
            writeNull(node, out);
        else if(node.isBool())
            writeBool(node, out);
        else if(node.isInt())
            writeInt(node, out);
        else if(node.isFloat())
            writeFloat(node, out);
        else if(node.isString())
            writeString(node, out);
        else if(node.isArray())
            writeArray(node, out, depth);
        else if(node.isDict())
            writeDict(node, out, depth);
    }
    
    void Writer::writeNull(const Node &node, std::ostream &out)
    {
        out << "null";
    }
    
    void Writer::writeBool(const Node &node, std::ostream &out)
    {
        out << (node.asBool() ? "true" : "false");
    }
    
    void Writer::writeInt(const Node &node, std::ostream &out)
    {
        out << node.asInteger();
    }
    
    void Writer::writeFloat(const Node &node, std::ostream &out)
    {
        out << node.asFloat();
    }
    
    void Writer::writeString(const Node &node, std::ostream &out)
    {
        const char *begin = node.asCString();
        const char *end = begin + node.size();
        
        out << "\"";
        for(const char *p = begin; p != end; ++p)
        {
            switch(*p)
            {
            case '\n':
                out << "\\n";
                break;
            case '\t':
                out << "\\t";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\\':
                out << "\\";
                break;
            case '\b':
                out << "\\b";
                break;
            case '\f':
                out << "\\f";
                break;
            case '\"':
                out << "\"";
                break;
            case '\0':
                out << "\\0";
                break;
            default:
                out << *p;
                break;
            }
        }
        out << "\"";
    }
    
    void Writer::writeArray(const Node &node, std::ostream &out, int depth)
    {
        if(node.size() == 0)
        {
            out << "[]";
            return;
        }
        
        out << "[" << eol_;
        SizeType n = (SizeType)node.size();
        for(SizeType i = 0; i < n; ++i)
        {
            out << Tab(depth + 1, tab_);
            writeNode(node[i], out, depth + 1);
            
            if(i + 1 != n)
                out << ',';
            out << eol_;
        }
        out << Tab(depth, tab_) << "]";
    }
    
    void Writer::writeDict(const Node &node, std::ostream &out, int depth)
    {
        if(node.size() == 0)
        {
            out << "{}";
            return;
        }
        
        out << "{" << eol_;
        const Dict *dict = node.asDict();
        for(Dict::const_iterator it = dict->begin(); it != dict->end(); ++it)
        {
            out << Tab(depth + 1, tab_);
            writeNode(it->key, out, depth + 1);
            out << ": ";
            writeNode(it->value, out, depth + 1);
            
            if(it + 1 != dict->end())
                out << ',';
            
            out << eol_;
        }
        
        out << Tab(depth, tab_) << "}";
    }
    
}
