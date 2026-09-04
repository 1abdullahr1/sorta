#include "ConflictDetector.h"
#include <QFileInfo>
#include <QSet>
#include <QMap>
#include <QRegularExpression>

ConflictReport ConflictDetector::validate(QVector<FileItem> &items)
{
    ConflictReport report;
    report.totalItems = items.size();

    // Map of normalized target path to list of indices
    QMap<QString, QVector<int>> targetPathIndices;
    // Set of current source paths being processed
    QSet<QString> sourcePaths;

    for (int i = 0; i < items.size(); ++i) {
        sourcePaths.insert(QFileInfo(items[i].filePath).absoluteFilePath().toLower());
    }

    for (int i = 0; i < items.size(); ++i) {
        FileItem &item = items[i];

        // Reset status
        item.status = item.isChanged() ? ItemStatus::Pending : ItemStatus::Unchanged;
        item.statusMessage.clear();

        if (!item.isChanged()) {
            continue;
        }

        report.changedCount++;

        // 1. Check valid Windows filename
        QString validationError;
        if (!isValidWindowsFileName(item.newFileName, validationError)) {
            item.status = ItemStatus::InvalidName;
            item.statusMessage = validationError;
            report.invalidNameCount++;
            report.hasErrors = true;
            continue;
        }

        // 2. Track destination path for batch collision detection
        QString target = QFileInfo(item.targetFullPath()).absoluteFilePath().toLower();
        targetPathIndices[target].append(i);
    }

    // Check duplicate target paths within the same batch
    for (auto it = targetPathIndices.begin(); it != targetPathIndices.end(); ++it) {
        const QVector<int> &indices = it.value();
        if (indices.size() > 1) {
            report.hasErrors = true;
            for (int idx : indices) {
                items[idx].status = ItemStatus::Conflict;
                items[idx].statusMessage = QString("Duplicate destination: %1 files share this new name.")
                                            .arg(indices.size());
                report.conflictCount++;
            }
        }
    }

    // Check disk collisions with files not in current batch
    for (int i = 0; i < items.size(); ++i) {
        FileItem &item = items[i];
        if (item.status != ItemStatus::Pending) {
            continue; // Already flagged
        }

        QString target = QFileInfo(item.targetFullPath()).absoluteFilePath();
        QString lowerTarget = target.toLower();

        // If target exists on disk
        if (QFileInfo::exists(target)) {
            // If the existing file is one of the files in our batch that is also being moved/renamed, it might be an in-place rename
            if (lowerTarget == QFileInfo(item.filePath).absoluteFilePath().toLower()) {
                // Windows is case-insensitive. If changing case (e.g. file.txt -> FILE.TXT), it's permissible
                item.status = ItemStatus::Pending;
            } else if (!sourcePaths.contains(lowerTarget)) {
                item.status = ItemStatus::TargetExists;
                item.statusMessage = "A file with this name already exists in the destination folder.";
                report.existingCollisionCount++;
                report.hasErrors = true;
            }
        }
    }

    return report;
}

bool ConflictDetector::isValidWindowsFileName(const QString &fileName, QString &errorMessage)
{
    if (fileName.trimmed().isEmpty()) {
        errorMessage = "File name cannot be empty.";
        return false;
    }

    if (fileName.length() > 255) {
        errorMessage = "File name exceeds 255 characters.";
        return false;
    }

    // Windows illegal characters: \ / : * ? " < > |
    static const QRegularExpression illegalChars(R"([<>:"/\\|?*])");
    if (illegalChars.match(fileName).hasMatch()) {
        errorMessage = "File name contains illegal characters: < > : \" / \\ | ? *";
        return false;
    }

    // Control characters (0 to 31)
    for (const QChar &ch : fileName) {
        if (ch.unicode() < 32) {
            errorMessage = "File name contains non-printable control characters.";
            return false;
        }
    }

    // Cannot end with space or dot on Windows
    if (fileName.endsWith(' ') || fileName.endsWith('.')) {
        errorMessage = "File name cannot end with a space or period on Windows.";
        return false;
    }

    // Check reserved names (CON, PRN, AUX, NUL, COM1-9, LPT1-9)
    QFileInfo fi(fileName);
    QString base = fi.completeBaseName();
    if (isReservedWindowsName(base)) {
        errorMessage = QString("'%1' is a reserved Windows device name.").arg(base.toUpper());
        return false;
    }

    return true;
}

bool ConflictDetector::isReservedWindowsName(const QString &baseName)
{
    static const QSet<QString> reserved = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
    };

    return reserved.contains(baseName.trimmed().toUpper());
}
