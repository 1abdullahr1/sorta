#ifndef RENAMEENGINE_H
#define RENAMEENGINE_H

#include <QString>
#include <QVector>
#include <QRegularExpression>
#include "FileItem.h"

enum class CaseConversion {
    None,
    LowerCase,
    UpperCase,
    TitleCase,
    SentenceCase,
    CamelCase
};

enum class NumberPosition {
    Suffix,
    Prefix,
    ReplaceName
};

enum class DatePosition {
    None,
    Prefix,
    Suffix
};

enum class DateSource {
    ModifiedDate,
    CurrentDate
};

struct RenameOptions {
    // Basic operations
    QString prefix;
    QString suffix;

    // Find & Replace
    bool enableReplace = false;
    QString findText;
    QString replaceText;
    bool matchCase = false;
    bool useRegex = false;

    // Remove characters
    bool enableRemove = false;
    QString removeText;

    // Case transformation
    CaseConversion caseChange = CaseConversion::None;

    // Sequential numbering
    bool enableNumbering = false;
    NumberPosition numberPos = NumberPosition::Suffix;
    int numberStart = 1;
    int numberStep = 1;
    int numberPadding = 3; // e.g. 001
    QString numberSeparator = "_";

    // Date
    DatePosition datePos = DatePosition::None;
    DateSource dateSource = DateSource::ModifiedDate;
    QString dateFormat = "yyyy-MM-dd";
    QString dateSeparator = "_";

    // Extension modification
    bool changeExtension = false;
    QString newExtension;
    bool lowercaseExtension = false;
    bool uppercaseExtension = false;

    // Pattern mode
    bool usePattern = false;
    QString patternTemplate = "{name}_{num:3}";
};

class RenameEngine {
public:
    static void applyRules(QVector<FileItem> &items, const RenameOptions &options);
    static QString computeNewName(const FileItem &item, int index, const RenameOptions &options);

private:
    static QString applyCaseConversion(const QString &text, CaseConversion conv);
    static QString formatNumber(int number, int padding);
};

#endif // RENAMEENGINE_H
