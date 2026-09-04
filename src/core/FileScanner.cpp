#include "FileScanner.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

FileScanner::FileScanner(QObject *parent)
    : QThread(parent)
{
}

FileScanner::~FileScanner()
{
    cancelScan();
    wait();
}

void FileScanner::setDirectory(const QString &dirPath, bool recursive)
{
    m_targetDir = dirPath;
    m_directFiles.clear();
    m_recursive = recursive;
    m_cancelRequested = false;
}

void FileScanner::setDirectFiles(const QStringList &filePaths)
{
    m_directFiles = filePaths;
    m_targetDir.clear();
    m_cancelRequested = false;
}

void FileScanner::cancelScan()
{
    m_cancelRequested = true;
}

bool FileScanner::isCanceled() const
{
    return m_cancelRequested.loadRelaxed();
}

void FileScanner::run()
{
    emit scanStarted();

    QVector<FileItem> batch;
    batch.reserve(500);
    int totalCount = 0;

    if (!m_directFiles.isEmpty()) {
        for (const QString &path : m_directFiles) {
            if (m_cancelRequested.loadRelaxed()) {
                emit scanCanceled();
                return;
            }

            QFileInfo info(path);
            if (info.exists() && info.isFile()) {
                batch.append(FileItem(info));
                totalCount++;

                if (batch.size() >= 250) {
                    emit batchFound(batch);
                    batch.clear();
                    batch.reserve(500);
                    emit progressUpdated(totalCount, QString("Scanned %1 files...").arg(totalCount));
                }
            } else if (info.exists() && info.isDir()) {
                // If a directory was dropped, scan it
                QDirIterator it(path, QDir::Files | QDir::NoSymLinks,
                                m_recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
                while (it.hasNext()) {
                    if (m_cancelRequested.loadRelaxed()) {
                        emit scanCanceled();
                        return;
                    }
                    it.next();
                    batch.append(FileItem(it.fileInfo()));
                    totalCount++;

                    if (batch.size() >= 250) {
                        emit batchFound(batch);
                        batch.clear();
                        batch.reserve(500);
                        emit progressUpdated(totalCount, QString("Scanned %1 files...").arg(totalCount));
                    }
                }
            }
        }
    } else if (!m_targetDir.isEmpty()) {
        QDirIterator::IteratorFlags flags = m_recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
        QDirIterator it(m_targetDir, QDir::Files | QDir::NoSymLinks | QDir::Readable, flags);

        while (it.hasNext()) {
            if (m_cancelRequested.loadRelaxed()) {
                emit scanCanceled();
                return;
            }

            it.next();
            batch.append(FileItem(it.fileInfo()));
            totalCount++;

            if (batch.size() >= 250) {
                emit batchFound(batch);
                batch.clear();
                batch.reserve(500);
                emit progressUpdated(totalCount, QString("Scanned %1 files...").arg(totalCount));
            }
        }
    }

    if (!batch.isEmpty()) {
        emit batchFound(batch);
    }

    emit progressUpdated(totalCount, QString("Found %1 files.").arg(totalCount));
    emit scanFinished(totalCount);
}
