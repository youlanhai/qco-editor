#ifndef QRESOURCETREE_H
#define QRESOURCETREE_H

#include "config.h"
#include <QTreeWidget>

const int PathRoleIndex = Qt::UserRole + 1;

class EDITOR_DLL QResourceTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QResourceTree(QWidget *parent = 0);

    QString getItemPath(const QTreeWidgetItem *item) const;
    QString getItemFullPath(const QTreeWidgetItem *item) const;
    void setItemPath(QTreeWidgetItem *item, const QString &path);

    void setAssetPath(const QString &path){ assetPath_ = path; }

protected:
    virtual void dropEvent(QDropEvent * event) override;
    virtual QStringList mimeTypes() const override;
    virtual QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const override;
    virtual Qt::DropActions supportedDropActions () const override;

    bool processDropEvent(QDropEvent * event);

    QString     assetPath_;
};

#endif // QRESOURCETREE_H
