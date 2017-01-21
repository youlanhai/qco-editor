#ifndef QRESROUCETABLE_H
#define QRESROUCETABLE_H

#include "config.h"
#include <QTableWidget>

class EDITOR_DLL QResourceTable : public QTableWidget
{
    Q_OBJECT
public:
    QResourceTable(QWidget *parent = 0);

    QString getItemPath(const QTableWidgetItem *item) const;
    QString getItemFullPath(const QTableWidgetItem *item) const;
    void setItemPath(QTableWidgetItem *item, const QString &path);

    void setAssetPath(const QString &path){ assetPath_ = path; }
    // current path is relative to asset path
    void setCurrentPath(const QString &path){ currentPath_= path; }

protected:
    virtual void dropEvent(QDropEvent * event) override;
    virtual QStringList mimeTypes() const override;
    virtual QMimeData* mimeData(const QList<QTableWidgetItem*> items) const override;
    virtual Qt::DropActions supportedDropActions () const override;

    bool processDropEvent(QDropEvent * event);

    QString     currentPath_;
    QString     assetPath_;
};

#endif // QRESROUCETABLE_H
