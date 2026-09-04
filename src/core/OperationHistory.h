#ifndef OPERATIONHISTORY_H
#define OPERATIONHISTORY_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QObject>

struct HistoryRecord {
    QString originalPath;
    QString newPath;
    bool success = false;
};

struct OperationBatch {
    QString id;
    QDateTime timestamp;
    QString operationType; // "Rename" or "Organize"
    QString targetFolder;
    int itemsAffected = 0;
    bool isReverted = false;
    QVector<HistoryRecord> records;
};

class OperationHistory : public QObject {
    Q_OBJECT

public:
    explicit OperationHistory(QObject *parent = nullptr);

    void recordBatch(const OperationBatch &batch);
    const QVector<OperationBatch>& getBatches() const;
    bool canUndo() const;
    bool undoLast(QString &errorMessage);
    bool undoBatch(int index, QString &errorMessage);

    void loadFromFile();
    void saveToFile();

signals:
    void historyChanged();

private:
    QVector<OperationBatch> m_batches;
    QString getStoragePath() const;
};

#endif // OPERATIONHISTORY_H
