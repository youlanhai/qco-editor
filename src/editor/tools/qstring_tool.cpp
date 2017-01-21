#include "qstring_tool.h"

#include <QFileInfo>
#include <QDir>

namespace Editor
{
    static const QChar SplitChar = '/';

    QString joinPath(const QString &a, const QString &b)
    {
        QString ret = a;
        if(ret.size() > 0 && ret[ret.size() - 1] != SplitChar && !b.isEmpty())
        {
            ret.append(SplitChar);
        }
        ret += b;
        return ret;
    }

    QString getFileName(const QString &filePath)
    {
        QString tmp = filePath;
        if(!tmp.isEmpty() && tmp[tmp.size() - 1] == SplitChar)
        {
            tmp.remove(tmp.size() - 1, 1);
        }

        int index = tmp.lastIndexOf(SplitChar);
        if(index < 0)
        {
            return tmp;
        }
        else
        {
            return tmp.right(tmp.size() - (index + 1));
        }
    }

     QString getFilePath(const QString &path)
     {
         int from = -1;
         if(!path.isEmpty() && path[path.size() - 1] == SplitChar)
         {
             --from;
         }

         int index = path.lastIndexOf(SplitChar, from);
         if(index < 0)
         {
             return QString();
         }
         else
         {
             return path.left(index);
         }
     }

     QString getFileExt(const QString &path)
     {
         int index = path.lastIndexOf('.');
         if(index < 0)
         {
             return QString();
         }
         else
         {
             return path.right(path.size() - index);
         }
     }

     void removeFileExt(QString &path)
     {
         int index = path.lastIndexOf('.');
         if(index >= 0)
         {
             path.remove(index, path.size() - index);
         }
     }

     bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
     {
         QFileInfo srcFileInfo(srcFilePath);
         if (srcFileInfo.isDir())
         {
             QDir targetDir(tgtFilePath);
             targetDir.cdUp();
             if (!targetDir.mkdir(getFileName(tgtFilePath)))
             {
                 return false;
             }

             QDir sourceDir(srcFilePath);
             QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
             foreach (const QString &fileName, fileNames)
             {
                 const QString newSrcFilePath = joinPath(srcFilePath, fileName);
                 const QString newTgtFilePath = joinPath(tgtFilePath, fileName);

                 if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                     return false;
             }
             return true;
         }
         else
         {
             return QFile::copy(srcFilePath, tgtFilePath);
         }
     }


    bool isSubPathOf(const QString &childPath, const QString &parentPath)
    {
        return childPath.startsWith(parentPath);
    }

}

