#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include "OperationHistory.h"

class HistoryDialog : public QDialog {
    Q_OBJECT

public:
    explicit HistoryDialog(OperationHistory *history, QWidget *parent = nullptr);

private slots:
    void refreshTable();
    void onSelectionChanged();
    void undoSelected();

private:
    void setupUi();

    OperationHistory *m_history;
    QTableWidget *m_batchTable;
    QTableWidget *m_recordsTable;
    QPushButton *m_undoButton;
};

#endif // HISTORYDIALOG_H
