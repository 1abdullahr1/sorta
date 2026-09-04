#include "OrganizeEngine.h"
#include <QDir>
#include <QFileInfo>

const QMap<QString, QStringList>& OrganizeEngine::defaultCategoryMap()
{
    static const QMap<QString, QStringList> categories = {
        {"Pictures",  {"jpg", "jpeg", "png", "gif", "bmp", "webp", "svg", "ico", "tiff", "heic", "raw", "cr2", "nef"}},
        {"Documents", {"pdf", "doc", "docx", "txt", "rtf", "odt", "xls", "xlsx", "ppt", "pptx", "csv", "md"}},
        {"Videos",    {"mp4", "mkv", "avi", "mov", "wmv", "flv", "webm", "m4v", "mpg", "mpeg"}},
        {"Audio",     {"mp3", "wav", "flac", "aac", "ogg", "m4a", "wma", "aiff"}},
        {"Archives",  {"zip", "rar", "7z", "tar", "gz", "bz2", "xz", "iso"}},
        {"Code",      {"cpp", "h", "c", "hpp", "cs", "py", "js", "ts", "html", "css", "rs", "go", "java", "json", "xml", "yaml", "yml"}},
        {"Programs",  {"exe", "msi", "bat", "cmd", "ps1", "sh"}}
    };
    return categories;
}

QString OrganizeEngine::getCategoryForExtension(const QString &ext)
{
    QString lowerExt = ext.toLower().trimmed();
    if (lowerExt.startsWith('.')) {
        lowerExt.remove(0, 1);
    }

    const auto &catMap = defaultCategoryMap();
    for (auto it = catMap.begin(); it != catMap.end(); ++it) {
        if (it.value().contains(lowerExt)) {
            return it.key();
        }
    }

    return "Other";
}

void OrganizeEngine::applyOrganization(QVector<FileItem> &items, const OrganizeOptions &options, const QString &baseFolder)
{
    for (FileItem &item : items) {
        item.newDirectoryPath = computeDestinationFolder(item, options, baseFolder);
    }
}

QString OrganizeEngine::computeDestinationFolder(const FileItem &item, const OrganizeOptions &options, const QString &baseFolder)
{
    QString root = baseFolder.isEmpty() ? item.directoryPath : baseFolder;
    QDir dir(root);

    QString subFolder;

    switch (options.mode) {
        case OrganizeMode::ByCategory: {
            subFolder = getCategoryForExtension(item.extension);
            break;
        }

        case OrganizeMode::ByDate: {
            if (item.modifiedTime.isValid()) {
                if (options.dateGrouping == DateGrouping::Year) {
                    subFolder = item.modifiedTime.toString("yyyy");
                } else if (options.dateGrouping == DateGrouping::YearMonth) {
                    subFolder = item.modifiedTime.toString("yyyy-MM");
                } else {
                    subFolder = item.modifiedTime.toString("yyyy/MM");
                }
            } else {
                subFolder = "Unknown Date";
            }
            break;
        }

        case OrganizeMode::BySize: {
            double mb = static_cast<double>(item.fileSizeBytes) / (1024.0 * 1024.0);
            if (mb < 10.0) {
                subFolder = "Small (< 10 MB)";
            } else if (mb < 100.0) {
                subFolder = "Medium (10 - 100 MB)";
            } else if (mb < 1024.0) {
                subFolder = "Large (100 MB - 1 GB)";
            } else {
                subFolder = "Huge (> 1 GB)";
            }
            break;
        }

        case OrganizeMode::CustomRule: {
            for (const CustomOrganizeRule &rule : options.customRules) {
                bool matched = false;
                if (rule.conditionType == "ext_in") {
                    QStringList exts = rule.pattern.split(',', Qt::SkipEmptyParts);
                    for (QString e : exts) {
                        if (e.trimmed().remove('.').compare(item.extension, Qt::CaseInsensitive) == 0) {
                            matched = true;
                            break;
                        }
                    }
                } else if (rule.conditionType == "name_contains") {
                    if (item.baseName.contains(rule.pattern.trimmed(), Qt::CaseInsensitive)) {
                        matched = true;
                    }
                } else if (rule.conditionType == "name_starts") {
                    if (item.baseName.startsWith(rule.pattern.trimmed(), Qt::CaseInsensitive)) {
                        matched = true;
                    }
                }

                if (matched) {
                    subFolder = rule.targetSubfolder;
                    break;
                }
            }

            if (subFolder.isEmpty()) {
                subFolder = "Unmatched";
            }
            break;
        }
    }

    return dir.filePath(subFolder);
}
