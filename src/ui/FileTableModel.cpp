#include "FileTableModel.h"
#include <QFont>
#include <QBrush>

FileTableModel::FileTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int FileTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_items.size();
}

int FileTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return ColumnCount;
}

QVariant FileTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size()) {
        return QVariant();
    }

    const FileItem &item = m_items[index.row()];
    int col = index.column();

    if (role == Qt::DisplayRole) {
        switch (col) {
            case ColStatus:
                switch (item.status) {
                    case ItemStatus::Unchanged:    return "Unchanged";
                    case ItemStatus::Pending:      return item.isChanged() ? "Ready" : "Unchanged";
                    case ItemStatus::Renamed:      return "Renamed";
                    case ItemStatus::Organized:    return "Organized";
                    case ItemStatus::Conflict:     return "Conflict";
                    case ItemStatus::TargetExists: return "Target Exists";
                    case ItemStatus::InvalidName:  return "Invalid Name";
                    case ItemStatus::Error:        return "Error";
                }
                return QString();

            case ColOriginalName:
                return item.fileName;

            case ColNewName:
                return item.newFileName;

            case ColExtension:
                return item.extension.toUpper();

            case ColSize:
                return item.formattedSize();

            case ColDate:
                return item.formattedDate();

            case ColDestination:
                return item.newDirectoryPath.isEmpty() ? item.directoryPath : item.newDirectoryPath;
        }
    } else if (role == Qt::ForegroundRole) {
        if (col == ColStatus) {
            switch (item.status) {
                case ItemStatus::Pending:
                    return item.isChanged() ? QBrush(QColor("#2563eb")) : QBrush(QColor("#64748b"));
                case ItemStatus::Renamed:
                case ItemStatus::Organized:
                    return QBrush(QColor("#16a34a"));
                case ItemStatus::Conflict:
                case ItemStatus::InvalidName:
                case ItemStatus::Error:
                    return QBrush(QColor("#dc2626"));
                case ItemStatus::TargetExists:
                    return QBrush(QColor("#d97706"));
                default:
                    return QBrush(QColor("#64748b"));
            }
        } else if (col == ColNewName) {
            if (item.newFileName != item.fileName) {
                return QBrush(QColor("#16a34a")); // Green when changed
            }
        }
    } else if (role == Qt::FontRole) {
        if ((col == ColNewName && item.newFileName != item.fileName) || col == ColStatus) {
            QFont font;
            font.setBold(true);
            return font;
        }
    } else if (role == Qt::ToolTipRole) {
        if (!item.statusMessage.isEmpty()) {
            return item.statusMessage;
        }
        if (item.isChanged()) {
            return QString("%1 -> %2").arg(item.fileName, item.newFileName);
        }
        return item.filePath;
    } else if (role == Qt::TextAlignmentRole) {
        if (col == ColSize) {
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
        } else if (col == ColStatus || col == ColExtension) {
            return static_cast<int>(Qt::AlignCenter | Qt::AlignVCenter);
        }
    }

    return QVariant();
}

QVariant FileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case ColStatus:        return "Status";
            case ColOriginalName:  return "Current Name";
            case ColNewName:       return "New Name (Preview)";
            case ColExtension:     return "Ext";
            case ColSize:          return "Size";
            case ColDate:          return "Modified";
            case ColDestination:   return "Target Location";
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void FileTableModel::setItems(const QVector<FileItem> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

void FileTableModel::appendItems(const QVector<FileItem> &items)
{
    if (items.isEmpty()) return;

    int first = m_items.size();
    int last = first + items.size() - 1;

    beginInsertRows(QModelIndex(), first, last);
    m_items.append(items);
    endInsertRows();
}

void FileTableModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

const QVector<FileItem>& FileTableModel::items() const
{
    return m_items;
}

QVector<FileItem>& FileTableModel::itemsRef()
{
    return m_items;
}

const FileItem& FileTableModel::itemAt(int row) const
{
    return m_items[row];
}

void FileTableModel::updatePreview()
{
    if (m_items.isEmpty()) return;
    emit dataChanged(index(0, 0), index(m_items.size() - 1, ColumnCount - 1));
}
