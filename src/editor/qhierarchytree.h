#ifndef QHIERARCHYTREE_H
#define QHIERARCHYTREE_H

#include "config.h"
#include <QTreeWidget>

class EDITOR_DLL QHierarchyTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QHierarchyTree(QWidget *parent = 0);

signals:
    void signalItemDropped(const QList<QTreeWidgetItem*> &items);
    void signalFileDropped(QTreeWidgetItem *item, const QStringList &paths);

public slots:

protected:
    QStringList mimeTypes() const override;

    virtual void dropEvent(QDropEvent *event) override;

    bool processDropEvent(QDropEvent *event);
};

#endif // QHIERARCHYTREE_H
