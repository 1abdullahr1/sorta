#ifndef FILEITEM_H
#define FILEITEM_H

#include <QString>
#include <QDateTime>
#include <QFileInfo>

enum class ItemStatus {
    Unchanged,
    Pending,
    Renamed,
    Organized,
    Conflict,
    TargetExists,
    InvalidName,
    Error
};

struct FileItem {
    QString filePath;           // Full original path (e.g. D:/Photos/IMG_001.jpg)
    QString fileName;           // Original name with extension (e.g. IMG_001.jpg)
    QString baseName;           // Base name without extension (e.g. IMG_001)
    QString extension;          // Extension without dot (e.g. jpg)
    QString directoryPath;      // Parent directory (e.g. D:/Photos)
    qint64 fileSizeBytes = 0;   // Size in bytes
    QDateTime modifiedTime;     // Last modified timestamp

    QString newFileName;        // Computed new file name (with extension)
    QString newDirectoryPath;   // Computed new target directory (for organize)

    ItemStatus status = ItemStatus::Unchanged;
    QString statusMessage;

    FileItem() = default;

    explicit FileItem(const QFileInfo& info)
        : filePath(info.absoluteFilePath()),
          fileName(info.fileName()),
          baseName(info.completeBaseName()),
          extension(info.suffix()),
          directoryPath(info.absolutePath()),
          fileSizeBytes(info.size()),
          modifiedTime(info.lastModified()),
          newFileName(info.fileName()),
          newDirectoryPath(info.absolutePath()),
          status(ItemStatus::Unchanged)
    {
    }

    QString targetFullPath() const {
        QString dir = newDirectoryPath.isEmpty() ? directoryPath : newDirectoryPath;
        QString name = newFileName.isEmpty() ? fileName : newFileName;
        if (!dir.endsWith('/') && !dir.endsWith('\\')) {
            dir += '/';
        }
        return dir + name;
    }

    bool isChanged() const {
        return (newFileName != fileName) || (!newDirectoryPath.isEmpty() && newDirectoryPath != directoryPath);
    }

    QString formattedSize() const {
        const double bytes = static_cast<double>(fileSizeBytes);
        if (bytes < 1024.0) {
            return QString::number(bytes, 'f', 0) + " B";
        } else if (bytes < 1024.0 * 1024.0) {
            return QString::number(bytes / 1024.0, 'f', 1) + " KB";
        } else if (bytes < 1024.0 * 1024.0 * 1024.0) {
            return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MB";
        } else {
            return QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
        }
    }

    QString formattedDate() const {
        return modifiedTime.isValid() ? modifiedTime.toString("yyyy-MM-dd hh:mm") : QString();
    }
};

#endif // FILEITEM_H
