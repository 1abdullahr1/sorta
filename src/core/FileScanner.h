#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QThread>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QAtomicInteger>
#include "FileItem.h"

class FileScanner : public QThread {
    Q_OBJECT

public:
    explicit FileScanner(QObject *parent = nullptr);
    ~FileScanner() override;

    void setDirectory(const QString &dirPath, bool recursive = false);
    void setDirectFiles(const QStringList &filePaths);
    void cancelScan();
    bool isCanceled() const;

signals:
    void scanStarted();
    void batchFound(const QVector<FileItem> &items);
    void progressUpdated(int count, const QString &currentStatus);
    void scanFinished(int totalCount);
    void scanCanceled();

protected:
    void run() override;

private:
    void scanPath(const QString &path);

    QString m_targetDir;
    QStringList m_directFiles;
    bool m_recursive = false;
    QAtomicInteger<bool> m_cancelRequested{false};
};

#endif // FILESCANNER_H
