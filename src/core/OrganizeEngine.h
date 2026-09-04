#ifndef ORGANIZEENGINE_H
#define ORGANIZEENGINE_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QStringList>
#include "FileItem.h"

enum class OrganizeMode {
    ByCategory,
    ByDate,
    BySize,
    CustomRule
};

enum class DateGrouping {
    Year,           // 2026
    YearMonth,      // 2026-09
    YearSlashMonth  // 2026/09
};

struct CustomOrganizeRule {
    QString ruleName;
    QString conditionType; // "ext_in", "name_contains", "name_starts"
    QString pattern;       // e.g. "pdf,doc" or "invoice"
    QString targetSubfolder; // e.g. "Documents/Invoices"
};

struct OrganizeOptions {
    OrganizeMode mode = OrganizeMode::ByCategory;
    DateGrouping dateGrouping = DateGrouping::YearMonth;
    bool createCategoryFolders = true;
    QVector<CustomOrganizeRule> customRules;
};

class OrganizeEngine {
public:
    static void applyOrganization(QVector<FileItem> &items, const OrganizeOptions &options, const QString &baseFolder);
    static QString computeDestinationFolder(const FileItem &item, const OrganizeOptions &options, const QString &baseFolder);
    static QString getCategoryForExtension(const QString &ext);

    static const QMap<QString, QStringList>& defaultCategoryMap();
};

#endif // ORGANIZEENGINE_H
