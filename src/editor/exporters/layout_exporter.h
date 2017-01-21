#ifndef LAYOUT_EXPORTER_H
#define LAYOUT_EXPORTER_H

#include "../config.h"
#include "../importers/file_importer.h"

class JsonHandle;

namespace Editor
{
    class EDITOR_DLL LayoutExporter : public FileImporter
    {
    public:
        virtual bool isAccepted(const QString &srcPath) const override;
        virtual bool execute(const QString &srcPath, const QString &dstPath) override;

    private:

        bool exportLayoutContent(JsonHandle config);
        bool exportLayoutComponents(JsonHandle config);
    };
}
#endif //LAYOUT_EXPORTER_H
