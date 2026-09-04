#include "MainWindow.h"
#include "HistoryDialog.h"
#include "AboutDialog.h"
#include "ConflictDetector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QHeaderView>
#include <QStatusBar>
#include <QDir>
#include <QFile>
#include <QUuid>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_scanner(new FileScanner(this)),
      m_history(new OperationHistory(this))
{
    setWindowTitle("Sorta — High-Speed Bulk File Renamer & Organizer");
    setWindowIcon(QIcon(":/app.png"));
    resize(1180, 780);
    setAcceptDrops(true);

    setupUi();

    // Scanner connections
    connect(m_scanner, &FileScanner::scanStarted, this, [this]() {
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, 0); // Indeterminate
        statusBar()->showMessage("Scanning folder...");
        m_scanBtn->setText("Cancel");
    });

    connect(m_scanner, &FileScanner::batchFound, this, &MainWindow::onScanBatchFound);
    connect(m_scanner, &FileScanner::scanFinished, this, &MainWindow::onScanFinished);
    connect(m_scanner, &FileScanner::scanCanceled, this, [this]() {
        m_progressBar->setVisible(false);
        statusBar()->showMessage("Scanning canceled.");
        m_scanBtn->setText("Rescan");
    });
    connect(m_scanner, &FileScanner::progressUpdated, this, [this](int count, const QString &status) {
        statusBar()->showMessage(status);
    });

    connect(m_history, &OperationHistory::historyChanged, this, [this]() {
        m_undoBtn->setEnabled(m_history->canUndo());
    });
}

MainWindow::~MainWindow()
{
    if (m_scanner->isRunning()) {
        m_scanner->cancelScan();
        m_scanner->wait();
    }
}

void MainWindow::setupUi()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(12, 12, 12, 8);
    rootLayout->setSpacing(10);

    // --- TOP BAR: Folder Picker & Actions ---
    auto *topBarLayout = new QHBoxLayout();
    topBarLayout->setSpacing(8);

    m_selectFolderBtn = new QPushButton("Select Folder", this);
    m_selectFolderBtn->setObjectName("accentButton");

    m_folderEdit = new QLineEdit(this);
    m_folderEdit->setPlaceholderText("Select a folder or drag-and-drop files / folders here...");

    m_recursiveCheck = new QCheckBox("Include Subfolders", this);

    m_scanBtn = new QPushButton("Rescan", this);
    m_clearBtn = new QPushButton("Clear List", this);

    m_undoBtn = new QPushButton("Undo Last", this);
    m_undoBtn->setEnabled(m_history->canUndo());

    m_historyBtn = new QPushButton("History", this);
    m_aboutBtn = new QPushButton("About", this);

    topBarLayout->addWidget(m_selectFolderBtn);
    topBarLayout->addWidget(m_folderEdit, 1);
    topBarLayout->addWidget(m_recursiveCheck);
    topBarLayout->addWidget(m_scanBtn);
    topBarLayout->addWidget(m_clearBtn);
    topBarLayout->addWidget(m_undoBtn);
    topBarLayout->addWidget(m_historyBtn);
    topBarLayout->addWidget(m_aboutBtn);

    rootLayout->addLayout(topBarLayout);

    // --- MAIN SPLITTER: Left controls (Tabs), Right table ---
    auto *splitter = new QSplitter(Qt::Horizontal, this);

    // Left: Control Tabs
    m_actionTabs = new QTabWidget(splitter);
    m_renamePanel = new RenamePanel(m_actionTabs);
    m_organizePanel = new OrganizePanel(m_actionTabs);

    m_actionTabs->addTab(m_renamePanel, "Rename Rules");
    m_actionTabs->addTab(m_organizePanel, "Organize into Folders");
    m_actionTabs->setMinimumWidth(380);
    m_actionTabs->setMaximumWidth(480);
    splitter->addWidget(m_actionTabs);

    // Right: Table View with Search/Filter
    auto *rightWidget = new QWidget(splitter);
    auto *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(6);

    // Table filter input
    auto *filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Quick Search:", rightWidget));
    m_filterEdit = new QLineEdit(rightWidget);
    m_filterEdit->setPlaceholderText("Filter files by name or extension...");
    m_filterEdit->setClearButtonEnabled(true);
    filterLayout->addWidget(m_filterEdit, 1);
    rightLayout->addLayout(filterLayout);

    // Table setup
    m_tableView = new QTableView(rightWidget);
    m_tableModel = new FileTableModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(FileTableModel::ColOriginalName);

    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColStatus, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColOriginalName, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColNewName, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColExtension, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColSize, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColDate, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColDestination, QHeaderView::Stretch);

    rightLayout->addWidget(m_tableView);
    splitter->addWidget(rightWidget);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    rootLayout->addWidget(splitter, 1);

    // --- BOTTOM ACTION BAR ---
    auto *bottomLayout = new QHBoxLayout();
    m_summaryLabel = new QLabel("No files loaded.", this);
    m_summaryLabel->setStyleSheet("font-weight: 600; color: #334155;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedWidth(200);

    m_applyBtn = new QPushButton("Apply Changes", this);
    m_applyBtn->setObjectName("primaryButton");
    m_applyBtn->setMinimumWidth(150);
    m_applyBtn->setEnabled(false);

    bottomLayout->addWidget(m_summaryLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_progressBar);
    bottomLayout->addWidget(m_applyBtn);

    rootLayout->addLayout(bottomLayout);

    // Status bar
    statusBar()->showMessage("Ready. Select a folder or drag and drop files to get started.");

    // Signal connections
    connect(m_selectFolderBtn, &QPushButton::clicked, this, &MainWindow::chooseFolder);
    connect(m_scanBtn, &QPushButton::clicked, this, &MainWindow::startScan);
    connect(m_clearBtn, &QPushButton::clicked, this, &MainWindow::clearList);
    connect(m_undoBtn, &QPushButton::clicked, this, &MainWindow::executeUndoLast);
    connect(m_historyBtn, &QPushButton::clicked, this, &MainWindow::showHistoryDialog);
    connect(m_aboutBtn, &QPushButton::clicked, this, &MainWindow::showAboutDialog);
    connect(m_applyBtn, &QPushButton::clicked, this, &MainWindow::executeApply);

    connect(m_renamePanel, &RenamePanel::optionsChanged, this, &MainWindow::onRenameOptionsChanged);
    connect(m_organizePanel, &OrganizePanel::optionsChanged, this, &MainWindow::onOrganizeOptionsChanged);
    connect(m_actionTabs, &QTabWidget::currentChanged, this, &MainWindow::recomputePreview);

    connect(m_filterEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
}

void MainWindow::chooseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Folder to Organize", m_currentFolder);
    if (!dir.isEmpty()) {
        m_currentFolder = dir;
        m_folderEdit->setText(dir);
        startScan();
    }
}

void MainWindow::startScan()
{
    if (m_scanner->isRunning()) {
        m_scanner->cancelScan();
        return;
    }

    QString path = m_folderEdit->text().trimmed();
    if (path.isEmpty()) {
        chooseFolder();
        return;
    }

    if (!QDir(path).exists()) {
        QMessageBox::warning(this, "Folder Not Found", "The specified folder does not exist.");
        return;
    }

    m_currentFolder = path;
    m_tableModel->clear();
    m_scanner->setDirectory(path, m_recursiveCheck->isChecked());
    m_scanner->start();
}

void MainWindow::cancelScan()
{
    if (m_scanner->isRunning()) {
        m_scanner->cancelScan();
    }
}

void MainWindow::onScanBatchFound(const QVector<FileItem> &items)
{
    m_tableModel->appendItems(items);
    updateStatusBarCounts();
}

void MainWindow::onScanFinished(int total)
{
    m_progressBar->setVisible(false);
    m_scanBtn->setText("Rescan");
    statusBar()->showMessage(QString("Scanning completed. Found %1 files.").arg(total));
    recomputePreview();
}

void MainWindow::clearList()
{
    if (m_scanner->isRunning()) {
        m_scanner->cancelScan();
        m_scanner->wait();
    }
    m_tableModel->clear();
    m_folderEdit->clear();
    m_currentFolder.clear();
    updateStatusBarCounts();
    m_applyBtn->setEnabled(false);
    statusBar()->showMessage("List cleared.");
}

void MainWindow::onRenameOptionsChanged(const RenameOptions &)
{
    if (m_actionTabs->currentIndex() == 0) {
        recomputePreview();
    }
}

void MainWindow::onOrganizeOptionsChanged(const OrganizeOptions &)
{
    if (m_actionTabs->currentIndex() == 1) {
        recomputePreview();
    }
}

void MainWindow::recomputePreview()
{
    QVector<FileItem> &items = m_tableModel->itemsRef();
    if (items.isEmpty()) {
        updateStatusBarCounts();
        return;
    }

    if (m_actionTabs->currentIndex() == 0) {
        // Rename mode
        RenameOptions opt = m_renamePanel->getOptions();
        RenameEngine::applyRules(items, opt);
        // Reset destination to original directory
        for (FileItem &it : items) {
            it.newDirectoryPath = it.directoryPath;
        }
    } else {
        // Organize mode
        OrganizeOptions opt = m_organizePanel->getOptions();
        OrganizeEngine::applyOrganization(items, opt, m_currentFolder);
        // Reset newFileName to original fileName
        for (FileItem &it : items) {
            it.newFileName = it.fileName;
        }
    }

    // Safety and conflict checking
    m_lastReport = ConflictDetector::validate(items);

    m_tableModel->updatePreview();
    updateStatusBarCounts();
}

void MainWindow::updateStatusBarCounts()
{
    int total = m_tableModel->rowCount();
    int changed = m_lastReport.changedCount;
    int conflicts = m_lastReport.conflictCount + m_lastReport.existingCollisionCount + m_lastReport.invalidNameCount;

    QString summary = QString("Total: %1 files | To Process: %2").arg(total).arg(changed);
    if (conflicts > 0) {
        summary += QString(" | <span style='color:#dc2626;'><b>Conflicts: %1</b></span>").arg(conflicts);
        m_applyBtn->setEnabled(false);
    } else {
        m_applyBtn->setEnabled(changed > 0);
    }

    m_summaryLabel->setText(summary);
}

void MainWindow::executeApply()
{
    QVector<FileItem> &items = m_tableModel->itemsRef();
    if (items.isEmpty()) return;

    if (m_lastReport.hasErrors) {
        QMessageBox::critical(this, "Cannot Proceed",
                              "Please resolve all filename conflicts and invalid names before applying.");
        return;
    }

    QString actionName = (m_actionTabs->currentIndex() == 0) ? "Rename" : "Organize";
    auto reply = QMessageBox::question(this, QString("Confirm %1").arg(actionName),
                                       QString("Are you sure you want to process %1 files?\nThis will modify files on disk.")
                                       .arg(m_lastReport.changedCount),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    OperationBatch batch;
    batch.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    batch.timestamp = QDateTime::currentDateTime();
    batch.operationType = actionName;
    batch.targetFolder = m_currentFolder;
    batch.itemsAffected = 0;
    batch.isReverted = false;

    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, items.size());
    m_progressBar->setValue(0);

    int successCount = 0;
    int errorCount = 0;

    for (int i = 0; i < items.size(); ++i) {
        FileItem &item = items[i];
        m_progressBar->setValue(i + 1);

        if (!item.isChanged()) {
            continue;
        }

        QString targetPath = item.targetFullPath();
        QString targetDir = item.newDirectoryPath.isEmpty() ? item.directoryPath : item.newDirectoryPath;

        // Ensure target directory exists
        QDir dir;
        if (!dir.exists(targetDir)) {
            if (!dir.mkpath(targetDir)) {
                item.status = ItemStatus::Error;
                item.statusMessage = "Could not create target directory.";
                errorCount++;
                continue;
            }
        }

        QFile file(item.filePath);
        if (file.rename(targetPath)) {
            HistoryRecord rec;
            rec.originalPath = item.filePath;
            rec.newPath = targetPath;
            rec.success = true;
            batch.records.append(rec);

            item.filePath = targetPath;
            item.fileName = item.newFileName;
            item.directoryPath = targetDir;
            item.baseName = QFileInfo(targetPath).completeBaseName();
            item.extension = QFileInfo(targetPath).suffix();
            item.status = (actionName == "Rename") ? ItemStatus::Renamed : ItemStatus::Organized;
            successCount++;
        } else {
            item.status = ItemStatus::Error;
            item.statusMessage = file.errorString();
            errorCount++;
        }
    }

    batch.itemsAffected = successCount;
    if (successCount > 0) {
        m_history->recordBatch(batch);
    }

    m_progressBar->setVisible(false);
    m_tableModel->updatePreview();
    updateStatusBarCounts();

    if (errorCount == 0) {
        QMessageBox::information(this, "Success",
                                 QString("Successfully processed %1 files.").arg(successCount));
    } else {
        QMessageBox::warning(this, "Completed with Errors",
                             QString("Processed %1 files, but %2 files failed.\nCheck the table for error details.")
                             .arg(successCount).arg(errorCount));
    }
}

void MainWindow::executeUndoLast()
{
    QString errorMsg;
    if (m_history->undoLast(errorMsg)) {
        QMessageBox::information(this, "Undo Successful", "Last operation was restored successfully.");
        // Rescan folder to reflect changes
        if (!m_currentFolder.isEmpty()) {
            startScan();
        }
    } else {
        QMessageBox::warning(this, "Undo Failed", errorMsg);
    }
}

void MainWindow::showHistoryDialog()
{
    HistoryDialog dlg(m_history, this);
    dlg.exec();
    // After history dialog, can rescan if an undo occurred
    if (!m_currentFolder.isEmpty()) {
        startScan();
    }
}

void MainWindow::showAboutDialog()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    m_proxyModel->setFilterRegularExpression(QRegularExpression::escape(text));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QStringList paths;
    for (const QUrl &url : urls) {
        paths.append(url.toLocalFile());
    }

    if (paths.size() == 1 && QFileInfo(paths.first()).isDir()) {
        m_currentFolder = paths.first();
        m_folderEdit->setText(m_currentFolder);
        startScan();
    } else {
        // Direct files / folders list
        m_tableModel->clear();
        m_currentFolder = QFileInfo(paths.first()).absolutePath();
        m_folderEdit->setText(m_currentFolder);
        m_scanner->setDirectFiles(paths);
        m_scanner->start();
    }

    event->acceptProposedAction();
}
