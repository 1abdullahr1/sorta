#include "HistoryDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>

HistoryDialog::HistoryDialog(OperationHistory *history, QWidget *parent)
    : QDialog(parent), m_history(history)
{
    setWindowTitle("Operation History & Undo — Sorta");
    resize(850, 550);
    setupUi();
    refreshTable();
}

void HistoryDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *headerLabel = new QLabel("Recent file operations are listed below. Select an operation to inspect details or revert changes.", this);
    headerLabel->setStyleSheet("color: #64748b; font-size: 13px; margin-bottom: 4px;");
    mainLayout->addWidget(headerLabel);

    auto *splitter = new QSplitter(Qt::Vertical, this);

    // Top: Batches
    auto *batchWidget = new QWidget(splitter);
    auto *batchLayout = new QVBoxLayout(batchWidget);
    batchLayout->setContentsMargins(0, 0, 0, 0);

    auto *batchTitle = new QLabel("Past Batches:", batchWidget);
    batchTitle->setStyleSheet("font-weight: 600;");
    batchLayout->addWidget(batchTitle);

    m_batchTable = new QTableWidget(0, 5, batchWidget);
    m_batchTable->setHorizontalHeaderLabels({"Timestamp", "Type", "Folder", "Files Affected", "Status"});
    m_batchTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_batchTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_batchTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_batchTable->setSelectionMode(QAbstractItemView::SingleSelection);
    batchLayout->addWidget(m_batchTable);
    splitter->addWidget(batchWidget);

    // Bottom: Detailed records
    auto *recWidget = new QWidget(splitter);
    auto *recLayout = new QVBoxLayout(recWidget);
    recLayout->setContentsMargins(0, 0, 0, 0);

    auto *recTitle = new QLabel("Files in Selected Operation:", recWidget);
    recTitle->setStyleSheet("font-weight: 600;");
    recLayout->addWidget(recTitle);

    m_recordsTable = new QTableWidget(0, 2, recWidget);
    m_recordsTable->setHorizontalHeaderLabels({"Original Path", "New Path"});
    m_recordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_recordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recLayout->addWidget(m_recordsTable);
    splitter->addWidget(recWidget);

    mainLayout->addWidget(splitter);

    // Buttons
    auto *btnLayout = new QHBoxLayout();
    m_undoButton = new QPushButton("Undo Selected Operation", this);
    m_undoButton->setObjectName("dangerButton");
    m_undoButton->setEnabled(false);

    auto *closeBtn = new QPushButton("Close", this);

    btnLayout->addWidget(m_undoButton);
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_batchTable, &QTableWidget::itemSelectionChanged, this, &HistoryDialog::onSelectionChanged);
    connect(m_undoButton, &QPushButton::clicked, this, &HistoryDialog::undoSelected);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void HistoryDialog::refreshTable()
{
    m_batchTable->setRowCount(0);
    m_recordsTable->setRowCount(0);

    const auto &batches = m_history->getBatches();
    m_batchTable->setRowCount(batches.size());

    for (int r = 0; r < batches.size(); ++r) {
        const OperationBatch &b = batches[r];
        m_batchTable->setItem(r, 0, new QTableWidgetItem(b.timestamp.toString("yyyy-MM-dd hh:mm:ss")));
        m_batchTable->setItem(r, 1, new QTableWidgetItem(b.operationType));
        m_batchTable->setItem(r, 2, new QTableWidgetItem(b.targetFolder));
        m_batchTable->setItem(r, 3, new QTableWidgetItem(QString::number(b.itemsAffected)));

        auto *statusItem = new QTableWidgetItem(b.isReverted ? "Reverted" : "Applied");
        if (b.isReverted) {
            statusItem->setForeground(QBrush(QColor("#64748b")));
        } else {
            statusItem->setForeground(QBrush(QColor("#16a34a")));
        }
        m_batchTable->setItem(r, 4, statusItem);
    }

    onSelectionChanged();
}

void HistoryDialog::onSelectionChanged()
{
    int row = m_batchTable->currentRow();
    const auto &batches = m_history->getBatches();

    if (row >= 0 && row < batches.size()) {
        const OperationBatch &b = batches[row];
        m_recordsTable->setRowCount(b.records.size());

        for (int i = 0; i < b.records.size(); ++i) {
            m_recordsTable->setItem(i, 0, new QTableWidgetItem(b.records[i].originalPath));
            m_recordsTable->setItem(i, 1, new QTableWidgetItem(b.records[i].newPath));
        }

        m_undoButton->setEnabled(!b.isReverted);
    } else {
        m_recordsTable->setRowCount(0);
        m_undoButton->setEnabled(false);
    }
}

void HistoryDialog::undoSelected()
{
    int row = m_batchTable->currentRow();
    if (row < 0) return;

    auto res = QMessageBox::question(this, "Confirm Undo",
                                     "Are you sure you want to revert all changes made in this operation?",
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (res != QMessageBox::Yes) return;

    QString errorMsg;
    if (m_history->undoBatch(row, errorMsg)) {
        QMessageBox::information(this, "Undo Successful", "All files in this operation were restored successfully.");
    } else {
        QMessageBox::warning(this, "Undo Warning", errorMsg);
    }

    refreshTable();
}
