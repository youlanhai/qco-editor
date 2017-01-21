#ifndef UILISTVIEWLOADER_H
#define UILISTVIEWLOADER_H

#include "scrollview_loader.h"

class PIP_DLL UIListViewLoader : public UIScrollViewLoader
{
public:
    DECLARE_LOADER(UIListViewLoader)
    UIListViewLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UILISTVIEWLOADER_H
