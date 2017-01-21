#ifndef UISCROLLVIEWLOADER_H
#define UISCROLLVIEWLOADER_H

#include "layout_loader.h"


class PIP_DLL UIScrollViewLoader : public UILayoutLoader
{
public:
    DECLARE_LOADER(UIScrollViewLoader)
    UIScrollViewLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UISCROLLVIEWLOADER_H
