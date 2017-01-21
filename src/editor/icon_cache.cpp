#include "icon_cache.h"
#include "framework.h"
#include "resource_mgr.h"
#include "project_mgr.h"

#include "tools/qstring_tool.h"
#include "tools/log_tool.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QBuffer>

namespace Editor
{
    const quint16 CacheFileVersion = 1;

    IconCache::IconCache(QObject *parent)
        : QObject(parent)
    {
        connect(&timer_, SIGNAL(timeout()), this, SLOT(slotStepLoad()));
        timer_.setInterval(1);
    }

    IconCache::~IconCache()
    {

    }

    QIcon IconCache::getFromCache(const QString &path, bool loadIfMiss)
    {
        auto it = cache_.find(path);
        if(it != cache_.end())
        {
            const IconInfo &iconInfo = it.value();
            QFileInfo fileInfo(iconInfo.fullPath);
            if(fileInfo.lastModified() < iconInfo.createTime)
            {
                return iconInfo.icon;
            }

            cache_.erase(it);
        }

        if(loadIfMiss)
        {
            addLoadTask(path);
        }
        return QIcon();
    }

    void IconCache::addLoadTask(const QString &path)
    {
        tasks_.push_back(path);

        if(tasks_.size() == 1)
        {
            timer_.start();
        }
    }

    void IconCache::clearCache()
    {
        cache_.clear();

        tasks_.clear();
        timer_.stop();
    }

    void IconCache::clearTasks()
    {
        tasks_.clear();
        timer_.stop();
    }


    bool loadPixmapFromCache(QPixmap &pixmap, const QString &iconCachePath, const QDateTime &modifiedTime)
    {
        QFile file(iconCachePath);
        if(!file.open(QIODevice::ReadOnly))
        {
            return false;
        }

        QDataStream ss(&file);
        quint16 version, reserve;
        ss >> version >> reserve;

        // 校验版本
        if(version != CacheFileVersion)
        {
            return false;
        }

        ss.skipRawData(reserve);

        QDateTime saveTime;
        ss >> saveTime;

        //比对时间，icon数据是否已经过时
        if(saveTime < modifiedTime)
        {
            return false;
        }

        // 读取图片数据数据
        QByteArray data;
        ss >> data;
        if(!pixmap.loadFromData((uchar*)data.data(), data.size()))
        {
            return false;
        }
        return true;
    }

    void scalePixmap(QPixmap &pixmap)
    {
        QSize size = pixmap.size();
        int maxSize = 128;
        if(size.width() > maxSize || size.height() > maxSize)
        {
            float aspect = float(size.width()) / size.height();
            if(size.width() > size.height())
            {
                size.setWidth(maxSize);
                size.setHeight(maxSize / aspect);
            }
            else
            {
                size.setWidth(maxSize * aspect);
                size.setHeight(maxSize);
            }
            pixmap = pixmap.scaled(size);
        }
    }

    bool savePixmapToCache(QPixmap &pixmap, const QString &iconCachePath)
    {
        QFile file(iconCachePath);
        if(!file.open(QIODevice::WriteOnly))
        {
            return false;
        }

        QDataStream ss(&file);
        ss << (quint16)CacheFileVersion; //version
        ss << (quint16)0; //reserve
        ss << QDateTime::currentDateTime(); //create time

        QBuffer ioBuffer;
        if(!ioBuffer.open(QIODevice::WriteOnly) || !pixmap.save(&ioBuffer, "PNG", 100))
        {
            LOG_ERROR("Failed save icon cache '%s'.", iconCachePath.toUtf8().data());
            return false;
        }

        ss << ioBuffer.data();
        return true;
    }

    void IconCache::slotStepLoad()
    {
        QString path = tasks_.front();
        tasks_.pop_front();

        if(tasks_.empty())
        {
            timer_.stop();
        }

        QString cachePath = ProjectManager::instance()->getCachePath();
        QDir dir(cachePath);
        if(!dir.exists())
        {
            dir.mkdir(cachePath);
        }

        auto resMgr = Framework::instance()->resource_;
        QString fullPath = resMgr->toAbsolutePath(path);
        QFileInfo info(fullPath);
        if(!info.exists())
        {
            return;
        }

        QString uuid = resMgr->path2uuid(path);
        if(uuid.isEmpty())
        {
            return;
        }
        QString iconCachePath = joinPath(cachePath, uuid);

        QPixmap pixmap;
        if(!loadPixmapFromCache(pixmap, iconCachePath, info.lastModified()))
        {
            LOG_INFO("Load file icon '%s'", path.toUtf8().data());
            if(!pixmap.load(fullPath))
            {
                return;
            }

            scalePixmap(pixmap);

            if(!savePixmapToCache(pixmap, iconCachePath))
            {
                LOG_WARN("Failed to write icon cache '%s'", iconCachePath.toUtf8().data());
            }
        }

        IconInfo iconInfo;
        iconInfo.icon = QIcon(pixmap);
        iconInfo.fullPath = fullPath;
        iconInfo.createTime = QDateTime::currentDateTime();
        cache_[path] = iconInfo;

        emit signalIconLoaded(path, iconInfo.icon);
    }
}
