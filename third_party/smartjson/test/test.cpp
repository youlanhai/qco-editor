//
//  main.cpp
//  minijson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include <iostream>

#include "json.hpp"
#include "parser.hpp"
#include "writer.hpp"

#include <string>
#include <cassert>
#include <cmath>

#define TEST_EQUAL(EXP) testEqual(EXP, #EXP, __LINE__)
void testEqual(bool ret, const char *exp, int line)
{
    if(!ret)
    {
        std::cout << "TestFailed: (" << exp << "), line: " << line << std::endl;
        abort();
    }
}

bool almoseEqual(double a, double b, double epsilon = 0.00001)
{
    return std::abs(a - b) < epsilon;
}

void testString()
{
    std::cout << "test string..." << std::endl;
    
    mjson::IAllocator *allocator = new mjson::RawAllocator();
    allocator->retain();
    
    std::string str("Hello World");
    mjson::String *p = allocator->createString(str.c_str(), str.size());
    p->retain();
    
    TEST_EQUAL(p->getRefCount() == 1);
    TEST_EQUAL(p->size() == str.size());
    TEST_EQUAL(str == p->data());
    TEST_EQUAL(p->compare(str.c_str()) == 0);
    
    mjson::String *p2 = (mjson::String*)p->clone();
    p2->retain();
    TEST_EQUAL(p->compare(p2) == 0);
    p2->release();
    
    mjson::String *p3 = (mjson::String*)p->deepClone();
    p3->retain();
    TEST_EQUAL(p->compare(p3) == 0);
    p3->release();
    
    p->release();
    
    allocator->release();
}

void testArray()
{
    std::cout << "test array..." << std::endl;
    
    
    mjson::RawAllocator allocator;
    allocator.retain();
    
    mjson::Array *p = allocator.createArray();
    p->retain();
    
    TEST_EQUAL(p->size() == 0);
    TEST_EQUAL(p->capacity() == 0);
    TEST_EQUAL(p->begin() == p->end());
    TEST_EQUAL(p->begin() == nullptr);
    
    p->append(true);
    p->append(1234567890);
    p->append(3.14);
    p->append("Hello World");
    TEST_EQUAL(p->size() == 4);
    TEST_EQUAL((*p)[0] == true);
    TEST_EQUAL((*p)[1] == 1234567890);
    TEST_EQUAL((*p)[2] == 3.14);
    TEST_EQUAL(strcmp((*p)[3].asCString(), "Hello World") == 0);
    
    TEST_EQUAL(p->capacity() == 4);
    p->append(mjson::Node());
    TEST_EQUAL(p->capacity() == 8);
    
    (*p)[3] = 1234;
    (*p)[4] = 1314;
    TEST_EQUAL((*p)[3] == 1234);
    TEST_EQUAL((*p)[4] == 1314);
    
    mjson::Array *copy = (mjson::Array*)p->clone();
    copy->retain();
    
    TEST_EQUAL(copy->size() == p->size());
    TEST_EQUAL(copy->capacity() == p->size());
    for(int i = 0; i < p->size(); ++i)
    {
        TEST_EQUAL((*copy)[i] == (*p)[i]);
    }
    
    copy->release();
    p->release();
}

void testDict()
{
    std::cout << "test dict..." << std::endl;
    
    mjson::RawAllocator allocator;
    allocator.retain();
    
    mjson::Dict *p = allocator.createDict();
    p->retain();
    
    (*p)["name"] = "json";
    (*p)["age"] = 20;
    (*p)["weight"] = 60.5;
    TEST_EQUAL(p->size() == 3);
    TEST_EQUAL(p->capacity() == 4);
    
    TEST_EQUAL((*p)["name"] == "json");
    TEST_EQUAL((*p)["age"] == 20);
    TEST_EQUAL((*p)["weight"] == 60.5);
    
    (*p)["name"] = "smart json";
    TEST_EQUAL((*p)["name"] == "smart json");
    
    mjson::Dict::iterator it = p->find("address");
    TEST_EQUAL(it == p->end());
    
    it = p->find("name");
    TEST_EQUAL(it != p->end());
    
    p->release();
}

void testNode()
{
    std::cout << "test node..." << std::endl;
    
    mjson::Node n0;
    TEST_EQUAL(n0.isNull());
    TEST_EQUAL(n0 == mjson::Node());
    
    mjson::Node n1(true);
    TEST_EQUAL(n1.isBool());
    TEST_EQUAL(n1.asBool() == true);
    
    mjson::Node n2(1234567890);
    TEST_EQUAL(n2.isInt());
    TEST_EQUAL(n2.isNumber());
    TEST_EQUAL(n2.asInteger() == 1234567890);
    
    mjson::Node n3(1234.5f);
    TEST_EQUAL(n3.isFloat());
    TEST_EQUAL(n3.isNumber());
    TEST_EQUAL(n3.asFloat() == 1234.5);
    
    mjson::Node n4(1234.5);
    TEST_EQUAL(n4.isFloat());
    TEST_EQUAL(n4.isNumber());
    TEST_EQUAL(n4.asFloat() == 1234.5);
    TEST_EQUAL(n4 == n3);
    TEST_EQUAL(n4 != n2);
    TEST_EQUAL(n4 != n1);
    TEST_EQUAL(n4 != n0);
    
    mjson::Node n5("Hello World");
    TEST_EQUAL(n5.isString());
    TEST_EQUAL(n5.isPointer());
    TEST_EQUAL(n5.asString() != 0);
    TEST_EQUAL(strcmp(n5.asCString(), "Hello World") == 0);
    TEST_EQUAL(n5 != n0);
    TEST_EQUAL(n5 != n1);
    TEST_EQUAL(n5 != n2);
    TEST_EQUAL(n5 != n3);
    TEST_EQUAL(n5 != n4);
    TEST_EQUAL(n5 == n5);
    TEST_EQUAL(n5.size() == n5.asString()->size());
    
    n5 = n5;
    TEST_EQUAL(n5.asString()->getRefCount() == 1);
    
    mjson::Node n6(n5);
    TEST_EQUAL(n6.isString());
    TEST_EQUAL(n6 == n5);
    TEST_EQUAL(n6.asString() == n5.asString());
    TEST_EQUAL(n6.asString()->getRefCount() == 2);
    
    n6 = n5;
    TEST_EQUAL(n6.isString());
    TEST_EQUAL(n6.asString()->getRefCount() == 2);
    TEST_EQUAL(n6.asString() == n5.asString());
    
    mjson::Node n7;
    n7.setArray();
    TEST_EQUAL(n7.isArray());
    n7.asArray()->append(n0);
    n7.asArray()->append(n1);
    n7.asArray()->append(n2);
    n7.asArray()->append(n3);
    n7.asArray()->append(n4);
    n7.asArray()->append(n5);
    n7.asArray()->append(n6);
    TEST_EQUAL(n7.asArray()->size() == n7.size());
    TEST_EQUAL(n7.size() == 7);
    TEST_EQUAL(n7[0u] == n0);
    for(mjson::Array::const_iterator it = n7.asArray()->begin();
        it != n7.asArray()->end(); ++it)
    {
        TEST_EQUAL(*it != n7);
    }
    for(mjson::SizeType i = 0; i < n7.size(); ++i)
    {
        TEST_EQUAL(n7[i] == n7.asArray()->at(i));
    }
    
    mjson::Node n8;
    n8.setDict();
    TEST_EQUAL(n8.isDict());
    n8["0"] = n0;
    n8["1"] = n1;
    n8["2"] = n2;
    n8["3"] = n3;
    n8["4"] = n4;
    n8["5"] = n5;
    n8["6"] = n6;
    n8["7"] = n7;
    TEST_EQUAL(n8.size() == 8);
    TEST_EQUAL(n8.size() == n8.rawDict()->size());
}

void testParser()
{
    std::cout << "test parser..." << std::endl;
    
    const char *json = "{\n"
    "\"name\"   : \"json\",\n"
    "\"age\"    : 20,\n"
    "\"weight\" : 60.5,\n"
    "\"i1\"     : 1234567890,\n"
    "\"i2\"     : -123456789,\n"
    "\"i3\"     : 0,\n"
    "\"f1\"     : 3.14e2,\n"
    "\"f2\"     : 3.14e-2,\n"
    "\"f3\"     : -0.3140e10,\n"
    "\"f4\"     : -0.314e-10,\n"
    "\"array\"  : [0, true, false, null, 123, -456, 3.14, \"hello\\n world!\"],\n"
    "\"pos\"    : {\"x\" : 100.55, \"y\" : 200.22}\n"
    "}";
    
    mjson::Parser parser;
    int ret = parser.parse(json, strlen(json));
    std::cout << "parse result:" << ret << std::endl;
    TEST_EQUAL(ret == mjson::RC_OK);
    
    mjson::Node root = parser.getRoot();
    TEST_EQUAL(root["name"] == "json");
    TEST_EQUAL(root["age"] == 20);
    TEST_EQUAL(root["weight"] == 60.5);
    TEST_EQUAL(root["i1"] == 1234567890);
    TEST_EQUAL(root["i2"] == -123456789);
    TEST_EQUAL(root["i3"] == 0);
    TEST_EQUAL(root["f1"] == 3.14e2);
    TEST_EQUAL(almoseEqual(root["f2"].asFloat(), 3.14e-2));
    TEST_EQUAL(almoseEqual(root["f3"].asFloat(), -0.314e10));
    TEST_EQUAL(almoseEqual(root["f4"].asFloat(), -0.314e-10));
    
    mjson::Node array = root["array"];
    TEST_EQUAL(array.isArray());
    TEST_EQUAL(array[0u] == 0);
    TEST_EQUAL(array[1] == true);
    TEST_EQUAL(array[2] == false);
    TEST_EQUAL(array[3].isNull());
    TEST_EQUAL(array[4] == 123);
    TEST_EQUAL(array[5] == -456);
    TEST_EQUAL(array[6] == 3.14);
    TEST_EQUAL(array[7] == "hello\n world!");
    
    mjson::Node pos = root["pos"];
    TEST_EQUAL(pos.isDict());
    TEST_EQUAL(pos["x"] == 100.55);
    TEST_EQUAL(pos["y"] == 200.22);
    
    std::cout << "print json:" << std::endl;
    mjson::Writer writer;
    writer.write(root, std::cout);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    
    testString();
    testArray();
    testDict();
    testNode();
    testParser();
    
    std::cout << "test finished." << std::endl;
    return 0;
}
