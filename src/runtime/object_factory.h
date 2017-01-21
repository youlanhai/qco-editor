#ifndef H_OBJECT_FACTORY_H
#define H_OBJECT_FACTORY_H

#include "pip_config.h"
#include "singleton.h"

#include <string>
#include <map>

class ObjectLoader;
class JsonHandle;

class PIP_DLL ObjectFactory : public Singleton<ObjectFactory>
{
public:
    ObjectFactory();
    ~ObjectFactory();

    void registerLoader(const std::string &type, ObjectLoader *loader);
    ObjectLoader* findLoader(const std::string &type);

protected:
    typedef std::map<std::string, ObjectLoader*> Loaders;
    Loaders         loaders_;
};

#endif // H_OBJECT_FACTORY_H
