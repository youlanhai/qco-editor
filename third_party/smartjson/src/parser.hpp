//
//  parser.hpp
//  smartjson
//
//  Created by youlanhai on 16/1/1.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include "node.hpp"

namespace mjson
{
    class Reader;
    
    enum ResultCode
    {
        RC_OK,
        RC_END_OF_FILE,
        RC_INVALID_JSON,
        RC_INVALID_DICT,
        RC_INVALID_KEY,
        RC_INVALID_ARRAY,
        RC_INVALID_STRING,
        RC_INVALID_NUMBER,
        RC_INVALID_NULL,
        RC_INVALID_TRUE,
        RC_INVALID_FALSE,
    };
    
    /** json parser.
     *  bnf: http://www.json.org/json-zh.html
     *  NOTICE: The format '\uxxxx' doesn't supported.
     */
    class SMARTJSON_DLL Parser
    {
    public:
        explicit Parser(IAllocator *allocator = 0);
        ~Parser();
        
        int parse(const char *str, size_t length);
        
        Node getRoot() const{ return root_; }
        int getErrorCode() const { return errorCode_; }
        int getErrorOffset() const { return errorOffset_; }

    private:
        
        int parseDict(Node &node, Reader &reader);
        int parseArray(Node &node, Reader &reader);
        int parseNumber(Node &node, Reader &reader);
        int parseString(Node &node, Reader &reader);
        int parseTrue(Node &node, Reader &reader);
        int parseFalse(Node &node, Reader &reader);
        int parseNull(Node &node, Reader &reader);
        int parseValue(Node &node, Reader &reader);
        
        Node            root_;
        IAllocator*     allocator_;
        int 			errorCode_;
        int 			errorOffset_;
    };
}

#endif /* parser_hpp */
