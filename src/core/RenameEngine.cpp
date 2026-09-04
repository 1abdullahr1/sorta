#include "RenameEngine.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

void RenameEngine::applyRules(QVector<FileItem> &items, const RenameOptions &options)
{
    for (int i = 0; i < items.size(); ++i) {
        items[i].newFileName = computeNewName(items[i], i, options);
    }
}

QString RenameEngine::computeNewName(const FileItem &item, int index, const RenameOptions &options)
{
    QString baseName = item.baseName;
    QString extension = item.extension;

    // Pattern Mode
    if (options.usePattern && !options.patternTemplate.trimmed().isEmpty()) {
        QString result = options.patternTemplate;

        // Replace {name}
        result.replace("{name}", baseName);

        // Replace {ext}
        result.replace("{ext}", extension);

        // Replace {date}
        QString dateStr = item.modifiedTime.isValid() ? item.modifiedTime.toString("yyyy-MM-dd") : QDate::currentDate().toString("yyyy-MM-dd");
        result.replace("{date}", dateStr);

        // Replace {time}
        QString timeStr = item.modifiedTime.isValid() ? item.modifiedTime.toString("hh-mm-ss") : QTime::currentTime().toString("hh-mm-ss");
        result.replace("{time}", timeStr);

        // Replace {parent}
        QDir parentDir(item.directoryPath);
        result.replace("{parent}", parentDir.dirName());

        // Replace {size}
        result.replace("{size}", item.formattedSize());

        // Replace {num} or {num:X}
        int curNum = options.numberStart + (index * options.numberStep);
        QRegularExpression numRegex(R"(\{num(?::(\d+))?\})");
        QRegularExpressionMatchIterator it = numRegex.globalMatch(result);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            int pad = options.numberPadding;
            if (match.capturedLength(1) > 0) {
                pad = match.captured(1).toInt();
            }
            result.replace(match.captured(0), formatNumber(curNum, pad));
        }

        // Apply extension rules
        if (options.changeExtension) {
            extension = options.newExtension.trimmed();
            if (extension.startsWith('.')) {
                extension.remove(0, 1);
            }
        }
        if (options.lowercaseExtension) {
            extension = extension.toLower();
        } else if (options.uppercaseExtension) {
            extension = extension.toUpper();
        }

        // If template doesn't explicitly mention {ext} and extension is present, append it
        if (!options.patternTemplate.contains("{ext}") && !extension.isEmpty()) {
            return result + "." + extension;
        }

        return result;
    }

    // Standard Rule Pipeline
    // 1. Remove text
    if (options.enableRemove && !options.removeText.isEmpty()) {
        baseName.remove(options.removeText, options.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }

    // 2. Find & Replace
    if (options.enableReplace && !options.findText.isEmpty()) {
        if (options.useRegex) {
            QRegularExpression::PatternOptions regexOptions = options.matchCase 
                ? QRegularExpression::NoPatternOption 
                : QRegularExpression::CaseInsensitiveOption;
            QRegularExpression re(options.findText, regexOptions);
            baseName.replace(re, options.replaceText);
        } else {
            baseName.replace(options.findText, options.replaceText, 
                             options.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
        }
    }

    // 3. Case conversion
    if (options.caseChange != CaseConversion::None) {
        baseName = applyCaseConversion(baseName, options.caseChange);
    }

    // 4. Prefix & Suffix
    if (!options.prefix.isEmpty()) {
        baseName = options.prefix + baseName;
    }
    if (!options.suffix.isEmpty()) {
        baseName = baseName + options.suffix;
    }

    // 5. Sequential numbering
    if (options.enableNumbering) {
        int curNum = options.numberStart + (index * options.numberStep);
        QString numStr = formatNumber(curNum, options.numberPadding);

        switch (options.numberPos) {
            case NumberPosition::Prefix:
                baseName = numStr + options.numberSeparator + baseName;
                break;
            case NumberPosition::Suffix:
                baseName = baseName + options.numberSeparator + numStr;
                break;
            case NumberPosition::ReplaceName:
                baseName = numStr;
                break;
        }
    }

    // 6. Date stamp
    if (options.datePos != DatePosition::None) {
        QDateTime dt = (options.dateSource == DateSource::CurrentDate) 
            ? QDateTime::currentDateTime() 
            : (item.modifiedTime.isValid() ? item.modifiedTime : QDateTime::currentDateTime());
        QString dateFormatted = dt.toString(options.dateFormat.isEmpty() ? "yyyy-MM-dd" : options.dateFormat);

        if (options.datePos == DatePosition::Prefix) {
            baseName = dateFormatted + options.dateSeparator + baseName;
        } else if (options.datePos == DatePosition::Suffix) {
            baseName = baseName + options.dateSeparator + dateFormatted;
        }
    }

    // 7. Extension manipulation
    if (options.changeExtension) {
        extension = options.newExtension.trimmed();
        if (extension.startsWith('.')) {
            extension.remove(0, 1);
        }
    }
    if (options.lowercaseExtension) {
        extension = extension.toLower();
    } else if (options.uppercaseExtension) {
        extension = extension.toUpper();
    }

    if (extension.isEmpty()) {
        return baseName;
    }
    return baseName + "." + extension;
}

QString RenameEngine::formatNumber(int number, int padding)
{
    QString s = QString::number(number);
    while (s.length() < padding) {
        s.prepend('0');
    }
    return s;
}

QString RenameEngine::applyCaseConversion(const QString &text, CaseConversion conv)
{
    switch (conv) {
        case CaseConversion::LowerCase:
            return text.toLower();

        case CaseConversion::UpperCase:
            return text.toUpper();

        case CaseConversion::TitleCase: {
            QString result = text;
            bool newWord = true;
            for (int i = 0; i < result.size(); ++i) {
                if (result[i].isSpace() || result[i] == '_' || result[i] == '-' || result[i] == '.') {
                    newWord = true;
                } else if (newWord) {
                    result[i] = result[i].toUpper();
                    newWord = false;
                } else {
                    result[i] = result[i].toLower();
                }
            }
            return result;
        }

        case CaseConversion::SentenceCase: {
            QString result = text.toLower();
            if (!result.isEmpty()) {
                result[0] = result[0].toUpper();
            }
            return result;
        }

        case CaseConversion::CamelCase: {
            QString result;
            bool capitalizeNext = false;
            for (int i = 0; i < text.size(); ++i) {
                QChar c = text[i];
                if (c.isSpace() || c == '_' || c == '-') {
                    capitalizeNext = true;
                } else {
                    if (result.isEmpty()) {
                        result.append(c.toLower());
                    } else if (capitalizeNext) {
                        result.append(c.toUpper());
                        capitalizeNext = false;
                    } else {
                        result.append(c.toLower());
                    }
                }
            }
            return result;
        }

        default:
            return text;
    }
}
