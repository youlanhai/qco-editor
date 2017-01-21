//
//  parser.cpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#include "parser.hpp"

#include "container/string.hpp"
#include "container/array.hpp"
#include "container/dict.hpp"
#include "container/allocator.hpp"

#include <cmath>
#include <iostream>

namespace mjson
{
    class Reader
    {
    public:
        Reader(const char *begin, const char *end)
        : begin_(begin)
        , p_(begin)
        , end_(end)
        {}
        
        char read()
        {
            if(p_ < end_)
            {
                return *p_++;
            }
            return 0;
        }
        
        void unget()
        {
            p_ -= 1;
        }
        
        bool empty() const
        {
            return p_ >= end_;
        }
        
        const char* begin() const { return begin_; }
        const char* current() const { return p_; }
        
    private:
        const char*     p_;
        const char*     begin_;
        const char*     end_;
    };

    
    bool isWhiteSpace(char ch)
    {
        return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
    }
    
    char skipWhiteSpace(Reader &reader)
    {
        char ch;
        do
        {
            ch = reader.read();
        }while(isWhiteSpace(ch));
        return ch;
    }
    
    char translateChar(char ch)
    {
        switch (ch)
        {
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 't': return '\t';
            case 'r': return '\r';
            case '"': return '"';
            case '\\': return '\\';
            case '/': return '/';
                
            case 'u':// doesn't support.
            default: return ch;
        }
    }
    
    
    Parser::Parser(IAllocator *allocator)
    : allocator_(allocator)
    , errorCode_(RC_OK)
    , errorOffset_(0)
    {
        if(allocator_ == 0)
        {
            allocator_ = RawAllocator::defaultAllocator();
        }
        allocator_->retain();
    }
    
    Parser::~Parser()
    {
        allocator_->release();
    }
    
    int Parser::parse(const char *str, size_t length)
    {
        root_.setNull();
        errorCode_ = RC_OK;
        errorOffset_ = 0;
        
        Reader reader(str, str + length);
        
        int ret = RC_OK;
        do
        {
            char firstChar = skipWhiteSpace(reader);
            if(firstChar != '{' && firstChar != '[')
            {
                ret = RC_INVALID_JSON;
                break;
            }
            
            reader.unget();
            ret = parseValue(root_, reader);
            if(ret != RC_OK)
            {
                break;
            }
            
            char endChar = skipWhiteSpace(reader);
            if(endChar != 0)
            {
                ret = RC_INVALID_JSON;
            }
        }while(0);
        
        if(ret != RC_OK)
        {
            errorCode_ = ret;
            errorOffset_ = reader.current() - str;

#if 0
            int line = 1;
            int col = 1;
            for(const char *p = str; p != reader.current(); ++p)
            {
                if(*p == '\n')
                {
                    ++line;
                    col = 1;
                }
                else
                {
                    ++col;
                }
            }
            std::cout << "parse error: " << ret
            << " line: " << line
            << " col: " << col
            << std::endl;
#endif
        }
        return ret;
    }
    
    int Parser::parseValue(Node &node, Reader &reader)
    {
        int ret = RC_OK;
        char ch = skipWhiteSpace(reader);
        switch (ch)
        {
            case '\0':
                ret = RC_END_OF_FILE;
                break;
                
            case '{':
                ret = parseDict(node, reader);
                break;
                
            case '[':
                ret = parseArray(node, reader);
                break;
                
            case '\"':
                ret = parseString(node, reader);
                break;
                
            case 'n':
                ret = parseNull(node, reader);
                break;
                
            case 't':
                ret = parseTrue(node, reader);
                break;
                
            case 'f':
                ret = parseFalse(node, reader);
                break;
                
            default:
                reader.unget();
                ret = parseNumber(node, reader);
                break;
        }
        return ret;
    }
    
    int Parser::parseDict(Node &node, Reader &reader)
    {
        node.setDict(allocator_);
        
        char ch = skipWhiteSpace(reader);
        if(ch == '}')
        {
            return RC_OK;
        }
        reader.unget();
        
        int ret;
        do
        {
            Node key, value;
            ret = parseValue(key, reader);
            if(ret != RC_OK)
            {
                break;
            }
        
            if(!key.isString())
            {
                ret = RC_INVALID_KEY;
                break;
            }
            
            char ch = skipWhiteSpace(reader);
            if(ch != ':')
            {
                ret = RC_INVALID_DICT;
                break;
            }
            
            ret = parseValue(value, reader);
            if(ret != RC_OK)
            {
                break;
            }
        
            node.rawDict()->append(key, value);
            
            ch = skipWhiteSpace(reader);
            if(ch == '}')
            {
                break;
            }
            else if(ch != ',')
            {
                ret = RC_INVALID_DICT;
            }
        }while(ret == RC_OK);
        
        return ret;
    }
    
    int Parser::parseArray(Node &node, Reader &reader)
    {
        node.setArray(allocator_);
        
        char ch = skipWhiteSpace(reader);
        if(ch == ']')
        {
            return RC_OK;
        }
        reader.unget();
        
        int ret;
        do
        {
            Node child;
            ret = parseValue(child, reader);
            if(ret != RC_OK)
            {
                break;
            }
        
            node.rawArray()->append(child);
            
            char ch = skipWhiteSpace(reader);
            if(ch == ']')
            {
                break;
            }
            else if(ch != ',')
            {
                ret = RC_INVALID_ARRAY;
            }
        }while(ret == RC_OK);
        
        return ret;
    }
    
    int Parser::parseNumber(Node &node, Reader &reader)
    {
        Integer value = 0;
        double real = 0;
        int exponent = 0;
        int sign = 1;
        bool useFloat = false;
        
        char ch = reader.read();
        if(ch == '-')
        {
            sign = -1;
            ch = reader.read();
        }
        
        if(ch < '0' || ch > '9')
            return RC_INVALID_NUMBER;
        
        if(ch != '0')
        {
            while(ch >= '0' && ch <= '9')
            {
                value = value * 10 + ch - '0';
                ch = reader.read();
            }
        }
        else
        {
            ch = reader.read();
        }
        
        if(ch == '.')
        {
            useFloat = true;
            double pos = 0.1;
            
            ch = reader.read();
            while(ch >= '0' && ch <= '9')
            {
                real += pos * (ch - '0');
                pos *= 0.1;
                ch = reader.read();
            }
            
            int expSign = 1;
            if(ch == 'e' || ch == 'E')
            {
                ch = reader.read();
                if(ch == '-')
                {
                    expSign = -1;
                    ch = reader.read();
                }
                else if(ch == '+')
                {
                    expSign = 1;
                    ch = reader.read();
                }
                
                while(ch >= '0' && ch <= '9')
                {
                    exponent = exponent * 10 + ch - '0';
                    ch = reader.read();
                }
            }
            exponent *= expSign;
        }
        
        if(!isWhiteSpace(ch) && ch != ',' && ch != ']' && ch != '}')
        {
            return RC_INVALID_NUMBER;
        }
        reader.unget();
        
        if(useFloat)
        {
            real = (double)value + real;
            if(exponent != 0)
            {
                real *= pow(10.0, exponent);
            }
            real *= sign;
            node = real;
        }
        else
        {
            value *= sign;
            node = value;
        }
        return RC_OK;
    }
    
    int Parser::parseString(Node &node, Reader &reader)
    {
        const char *begin = reader.current();

        char ch;
        while(1)
        {
            ch = reader.read();
            if(ch == '\\')
            {
                ch = reader.read();
                if(ch != 0)
                {
                    continue;
                }
            }

            if(ch == 0 || ch == '\"' || ch == '\n')
            {
                break;
            }
        }
        
        if(ch != '\"')
        {
            return RC_INVALID_STRING;
        }
        const char *end = reader.current() - 1; // *end = '"'
    
        int ret = RC_OK;
        char* buffer = (char*)allocator_->malloc(end - begin + 1);
        char *p = buffer;
        for(; begin < end && ret == RC_OK; ++p, ++begin)
        {
            if(*begin == '\\')
            {
                ++begin;
                *p = translateChar(*begin);
            }
            else
            {
                *p = *begin;
            }
        }
        *p = '\0';
        node = allocator_->createRawString(buffer, p - buffer);
        return RC_OK;
    }
    
    int Parser::parseTrue(Node &node, Reader &reader)
    {
        if(reader.read() == 'r' &&
           reader.read() == 'u' &&
           reader.read() == 'e')
        {
            node = true;
            return RC_OK;
        }
        return RC_INVALID_TRUE;
    }
    
    int Parser::parseFalse(Node &node, Reader &reader)
    {
        if(reader.read() == 'a' &&
           reader.read() == 'l' &&
           reader.read() == 's' &&
           reader.read() == 'e')
        {
            node = false;
            return RC_OK;
        }
        return RC_INVALID_FALSE;
    }
    
    int Parser::parseNull(Node &node, Reader &reader)
    {
        if(reader.read() == 'u' &&
           reader.read() == 'l' &&
           reader.read() == 'l')
        {
            node.setNull();
            return RC_OK;
        }
        return RC_INVALID_NULL;
    }
   
   
}
