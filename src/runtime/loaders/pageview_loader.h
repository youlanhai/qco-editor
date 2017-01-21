#ifndef UIPAGEVIEWLOADER_H
#define UIPAGEVIEWLOADER_H

#include "layout_loader.h"

class PIP_DLL UIPageViewLoader : public UILayoutLoader
{
public:
    DECLARE_LOADER(UIPageViewLoader)
    UIPageViewLoader();
    virtual ObjectType* createObject(const JsonHandle &config) override;
};

#endif // UIPAGEVIEWLOADER_H
