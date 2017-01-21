#ifndef ICON_CACHE_H
#define ICON_CACHE_H

#include "config.h"
#include <QObject>

#include <QStringList>
#include <QHash>
#include <QIcon>
#include <QTimer>
#include <QDateTime>

namespace Editor
{
    /**
     *  缓存图片文件的icon。
     *  由于图片文件可能很大，或者带有压缩数据，加载速度非常慢。
     *  IconCache将图片的最大尺寸限制在128x128，将处理过后的数据写入临时文件中，只要图片没有发生过修改，
     *  就无需重新处理。
     *  IconCache还在内存中缓存了已经加载的Icon文件，可以加速对Icon的获取，但是会浪费一定的内存。
     *  IconCache还可以分步骤去加载或生成Icon，避免了对UI线程造成长时间的卡顿。
     */
    class EDITOR_DLL IconCache : public QObject
    {
        Q_OBJECT
    public:
        IconCache(QObject *parent=0);
        ~IconCache();

        /**
         *  @brief 从缓存中获取Icon
         *  @param path          图片路径
         *  @param loadIfMiss    如果缓存中不存在时，是否创建加载任务。
         *  @return 如果缓存中存在，则直接返回icon，否则返回空。
         */
        QIcon getFromCache(const QString &path, bool loadIfMiss=false);

        void addLoadTask(const QString &path);

        /**
         *  @brief 清除缓存的icon和加载任务
         */
        void clearCache();

        void clearTasks();

    signals:
        void signalIconLoaded(const QString &path, const QIcon &icon);

    public slots:
        void slotStepLoad();

    protected:
        struct IconInfo
        {
            QIcon       icon;
            QDateTime   createTime;
            QString     fullPath;
        };

        QHash<QString, IconInfo> cache_;
        QStringList         tasks_;
        QTimer              timer_;
    };
}

#endif // ICON_CACHE_H
