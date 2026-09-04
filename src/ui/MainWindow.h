#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTabWidget>
#include <QSortFilterProxyModel>

#include "FileTableModel.h"
#include "FileScanner.h"
#include "RenamePanel.h"
#include "OrganizePanel.h"
#include "OperationHistory.h"
#include "ConflictDetector.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void chooseFolder();
    void startScan();
    void cancelScan();
    void onScanBatchFound(const QVector<FileItem> &items);
    void onScanFinished(int total);
    void clearList();

    void onRenameOptionsChanged(const RenameOptions &options);
    void onOrganizeOptionsChanged(const OrganizeOptions &options);
    void recomputePreview();

    void executeApply();
    void executeUndoLast();
    void showHistoryDialog();
    void showAboutDialog();
    void onFilterTextChanged(const QString &text);

private:
    void setupUi();
    void updateStatusBarCounts();

    // Top Bar
    QLineEdit *m_folderEdit;
    QCheckBox *m_recursiveCheck;
    QPushButton *m_selectFolderBtn;
    QPushButton *m_scanBtn;
    QPushButton *m_clearBtn;

    // Action Buttons
    QPushButton *m_undoBtn;
    QPushButton *m_historyBtn;
    QPushButton *m_aboutBtn;

    // Center Controls
    QTabWidget *m_actionTabs;
    RenamePanel *m_renamePanel;
    OrganizePanel *m_organizePanel;

    // Filter and Table
    QLineEdit *m_filterEdit;
    QTableView *m_tableView;
    FileTableModel *m_tableModel;
    QSortFilterProxyModel *m_proxyModel;

    // Bottom Action Bar
    QLabel *m_summaryLabel;
    QPushButton *m_applyBtn;
    QProgressBar *m_progressBar;

    // Core
    FileScanner *m_scanner;
    OperationHistory *m_history;
    ConflictReport m_lastReport;
    QString m_currentFolder;
};

#endif // MAINWINDOW_H
