#ifndef FILETABLEMODEL_H
#define FILETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QColor>
#include "FileItem.h"

class FileTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        ColStatus = 0,
        ColOriginalName,
        ColNewName,
        ColExtension,
        ColSize,
        ColDate,
        ColDestination,
        ColumnCount
    };

    explicit FileTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setItems(const QVector<FileItem> &items);
    void appendItems(const QVector<FileItem> &items);
    void clear();

    const QVector<FileItem>& items() const;
    QVector<FileItem>& itemsRef();
    const FileItem& itemAt(int row) const;

    void updatePreview();

private:
    QVector<FileItem> m_items;
};

#endif // FILETABLEMODEL_H
