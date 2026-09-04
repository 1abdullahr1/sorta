#ifndef CONFLICTDETECTOR_H
#define CONFLICTDETECTOR_H

#include <QVector>
#include <QString>
#include <QSet>
#include "FileItem.h"

struct ConflictReport {
    int totalItems = 0;
    int changedCount = 0;
    int conflictCount = 0;
    int existingCollisionCount = 0;
    int invalidNameCount = 0;
    bool hasErrors = false;
};

class ConflictDetector {
public:
    static ConflictReport validate(QVector<FileItem> &items);
    static bool isValidWindowsFileName(const QString &fileName, QString &errorMessage);
    static bool isReservedWindowsName(const QString &baseName);
};

#endif // CONFLICTDETECTOR_H
