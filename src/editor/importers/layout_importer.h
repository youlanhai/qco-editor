#ifndef LAYOUT_IMPORTER_H
#define LAYOUT_IMPORTER_H

#include "file_importer.h"

class JsonHandle;
namespace Editor
{
    class EDITOR_DLL LayoutImporter : public FileImporter
    {
    public:
        virtual bool isAccepted(const QString &srcPath) const override;
        virtual bool execute(const QString &srcPath, const QString &dstPath) override;

    private:
        bool importLayoutContent(JsonHandle config);
    };
}

#endif //LAYOUT_IMPORTER_H
