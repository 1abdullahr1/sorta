#include "OperationHistory.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

OperationHistory::OperationHistory(QObject *parent)
    : QObject(parent)
{
    loadFromFile();
}

void OperationHistory::recordBatch(const OperationBatch &batch)
{
    m_batches.prepend(batch);
    if (m_batches.size() > 50) {
        m_batches.resize(50);
    }
    saveToFile();
    emit historyChanged();
}

const QVector<OperationBatch>& OperationHistory::getBatches() const
{
    return m_batches;
}

bool OperationHistory::canUndo() const
{
    for (const OperationBatch &batch : m_batches) {
        if (!batch.isReverted) {
            return true;
        }
    }
    return false;
}

bool OperationHistory::undoLast(QString &errorMessage)
{
    for (int i = 0; i < m_batches.size(); ++i) {
        if (!m_batches[i].isReverted) {
            return undoBatch(i, errorMessage);
        }
    }
    errorMessage = "No reversible operations found in history.";
    return false;
}

bool OperationHistory::undoBatch(int index, QString &errorMessage)
{
    if (index < 0 || index >= m_batches.size()) {
        errorMessage = "Invalid history index.";
        return false;
    }

    OperationBatch &batch = m_batches[index];
    if (batch.isReverted) {
        errorMessage = "This operation has already been undone.";
        return false;
    }

    int failureCount = 0;
    // Iterate in reverse to safely undo moves and renames
    for (int i = batch.records.size() - 1; i >= 0; --i) {
        const HistoryRecord &record = batch.records[i];
        if (!record.success) continue;

        QFile currentFile(record.newPath);
        if (!currentFile.exists()) {
            failureCount++;
            continue;
        }

        // Ensure original parent directory exists
        QFileInfo originalInfo(record.originalPath);
        QDir().mkpath(originalInfo.absolutePath());

        // Move/rename file back
        if (!currentFile.rename(record.originalPath)) {
            failureCount++;
        }
    }

    batch.isReverted = true;
    saveToFile();
    emit historyChanged();

    if (failureCount > 0) {
        errorMessage = QString("Reverted operation, but %1 files could not be restored (may be moved or deleted).")
                       .arg(failureCount);
        return false;
    }

    return true;
}

QString OperationHistory::getStoragePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath("sorta_history.json");
}

void OperationHistory::loadFromFile()
{
    QFile file(getStoragePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return;

    m_batches.clear();
    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        OperationBatch batch;
        batch.id = obj["id"].toString();
        batch.timestamp = QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate);
        batch.operationType = obj["operationType"].toString();
        batch.targetFolder = obj["targetFolder"].toString();
        batch.itemsAffected = obj["itemsAffected"].toInt();
        batch.isReverted = obj["isReverted"].toBool();

        QJsonArray recArr = obj["records"].toArray();
        for (const QJsonValue &rVal : recArr) {
            QJsonObject rObj = rVal.toObject();
            HistoryRecord rec;
            rec.originalPath = rObj["originalPath"].toString();
            rec.newPath = rObj["newPath"].toString();
            rec.success = rObj["success"].toBool();
            batch.records.append(rec);
        }

        m_batches.append(batch);
    }
}

void OperationHistory::saveToFile()
{
    QJsonArray arr;
    for (const OperationBatch &batch : m_batches) {
        QJsonObject obj;
        obj["id"] = batch.id;
        obj["timestamp"] = batch.timestamp.toString(Qt::ISODate);
        obj["operationType"] = batch.operationType;
        obj["targetFolder"] = batch.targetFolder;
        obj["itemsAffected"] = batch.itemsAffected;
        obj["isReverted"] = batch.isReverted;

        QJsonArray recArr;
        for (const HistoryRecord &rec : batch.records) {
            QJsonObject rObj;
            rObj["originalPath"] = rec.originalPath;
            rObj["newPath"] = rec.newPath;
            rObj["success"] = rec.success;
            recArr.append(rObj);
        }
        obj["records"] = recArr;
        arr.append(obj);
    }

    QFile file(getStoragePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        file.close();
    }
}
