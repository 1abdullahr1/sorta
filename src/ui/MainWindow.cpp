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
#include <QFrame>

static QFrame* createCard(QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("cardFrame");
    card->setStyleSheet("QFrame#cardFrame { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; }");
    return card;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_scanner(new FileScanner(this)),
      m_history(new OperationHistory(this))
{
    setWindowTitle("Sorta — High-Speed Bulk File Renamer & Organizer");
    setWindowIcon(QIcon(":/app.png"));
    resize(1240, 820);
    setAcceptDrops(true);

    setupUi();

    // Scanner connections
    connect(m_scanner, &FileScanner::scanStarted, this, [this]() {
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, 0);
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
    connect(m_scanner, &FileScanner::progressUpdated, this, [this](int, const QString &status) {
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
    centralWidget->setObjectName("centralWidget");
    setCentralWidget(centralWidget);

    auto *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(16, 14, 16, 12);
    rootLayout->setSpacing(12);

    // =========================================================================
    // 1. TOP HEADER & ACTIONS
    // =========================================================================
    auto *headerCard = createCard(centralWidget);
    auto *headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(14, 10, 14, 10);
    headerLayout->setSpacing(12);

    // App Branding (Icon + Title)
    auto *brandLayout = new QHBoxLayout();
    brandLayout->setSpacing(10);
    auto *logoLabel = new QLabel(headerCard);
    QPixmap icon(":/app.png");
    if (!icon.isNull()) {
        logoLabel->setPixmap(icon.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    brandLayout->addWidget(logoLabel);

    auto *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(0);
    auto *titleText = new QLabel("Sorta", headerCard);
    titleText->setStyleSheet("font-size: 16px; font-weight: 700; color: #0f172a;");
    auto *subtitleText = new QLabel("Bulk File Renamer & Organizer", headerCard);
    subtitleText->setStyleSheet("font-size: 11px; color: #64748b;");
    titleLayout->addWidget(titleText);
    titleLayout->addWidget(subtitleText);
    brandLayout->addLayout(titleLayout);
    headerLayout->addLayout(brandLayout);

    headerLayout->addStretch();

    // Action buttons
    m_selectFolderBtn = new QPushButton("Select Folder", headerCard);
    m_selectFolderBtn->setObjectName("primaryButton");

    m_scanBtn = new QPushButton("Rescan", headerCard);
    m_scanBtn->setEnabled(false);

    m_clearBtn = new QPushButton("Clear List", headerCard);
    m_clearBtn->setEnabled(false);

    m_undoBtn = new QPushButton("Undo Last", headerCard);
    m_undoBtn->setEnabled(m_history->canUndo());

    m_historyBtn = new QPushButton("History", headerCard);
    m_aboutBtn = new QPushButton("About", headerCard);

    headerLayout->addWidget(m_selectFolderBtn);
    headerLayout->addWidget(m_scanBtn);
    headerLayout->addWidget(m_clearBtn);
    headerLayout->addWidget(m_undoBtn);
    headerLayout->addWidget(m_historyBtn);
    headerLayout->addWidget(m_aboutBtn);

    rootLayout->addWidget(headerCard);

    // =========================================================================
    // 2. FOLDER LOCATION BANNER
    // =========================================================================
    auto *bannerCard = createCard(centralWidget);
    auto *bannerLayout = new QHBoxLayout(bannerCard);
    bannerLayout->setContentsMargins(14, 8, 14, 8);
    bannerLayout->setSpacing(10);

    auto *folderIconLabel = new QLabel("📁", bannerCard);
    folderIconLabel->setStyleSheet("font-size: 14px;");
    bannerLayout->addWidget(folderIconLabel);

    m_folderPathLabel = new QLabel("No folder selected. Choose a folder or drag files into the window.", bannerCard);
    m_folderPathLabel->setStyleSheet("font-size: 12.5px; color: #475569; font-weight: 500;");
    bannerLayout->addWidget(m_folderPathLabel, 1);

    m_recursiveCheck = new QCheckBox("Include Subfolders", bannerCard);
    bannerLayout->addWidget(m_recursiveCheck);

    rootLayout->addWidget(bannerCard);

    // =========================================================================
    // 3. MAIN SPLITTER (Left: Rules Tabs | Right: Table / Empty State)
    // =========================================================================
    auto *splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->setChildrenCollapsible(false);

    // Left Panel: Tabs
    m_actionTabs = new QTabWidget(splitter);
    m_renamePanel = new RenamePanel(m_actionTabs);
    m_organizePanel = new OrganizePanel(m_actionTabs);

    m_actionTabs->addTab(m_renamePanel, "Rename Rules");
    m_actionTabs->addTab(m_organizePanel, "Organize into Folders");
    m_actionTabs->setMinimumWidth(460);
    splitter->addWidget(m_actionTabs);

    // Right Panel: Stacked Widget (Page 0 = Empty State, Page 1 = Table View)
    m_rightStack = new QStackedWidget(splitter);

    // Page 0: Empty State
    m_emptyStateWidget = createCard(m_rightStack);
    auto *emptyLayout = new QVBoxLayout(m_emptyStateWidget);
    emptyLayout->setContentsMargins(40, 60, 40, 60);
    emptyLayout->setSpacing(14);
    emptyLayout->setAlignment(Qt::AlignCenter);

    auto *bigFolderIcon = new QLabel("📂", m_emptyStateWidget);
    bigFolderIcon->setStyleSheet("font-size: 54px; color: #94a3b8;");
    bigFolderIcon->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(bigFolderIcon);

    auto *emptyTitle = new QLabel("No Files Loaded", m_emptyStateWidget);
    emptyTitle->setStyleSheet("font-size: 18px; font-weight: 700; color: #0f172a;");
    emptyTitle->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(emptyTitle);

    auto *emptyDesc = new QLabel("Drag and drop any folder or group of files here,<br>or click the button below to browse.", m_emptyStateWidget);
    emptyDesc->setStyleSheet("font-size: 13px; color: #64748b; line-height: 1.5;");
    emptyDesc->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(emptyDesc);

    auto *emptyBrowseBtn = new QPushButton("Choose Folder to Organize", m_emptyStateWidget);
    emptyBrowseBtn->setObjectName("primaryButton");
    emptyBrowseBtn->setFixedWidth(240);
    emptyBrowseBtn->setFixedHeight(38);
    emptyLayout->addWidget(emptyBrowseBtn, 0, Qt::AlignCenter);

    m_rightStack->addWidget(m_emptyStateWidget);

    // Page 1: Table Container
    m_tableContainerWidget = createCard(m_rightStack);
    auto *tableContainerLayout = new QVBoxLayout(m_tableContainerWidget);
    tableContainerLayout->setContentsMargins(12, 12, 12, 12);
    tableContainerLayout->setSpacing(10);

    // Search and Table Header
    auto *tableTopLayout = new QHBoxLayout();
    tableTopLayout->setSpacing(8);

    auto *searchLabel = new QLabel("🔍", m_tableContainerWidget);
    tableTopLayout->addWidget(searchLabel);

    m_filterEdit = new QLineEdit(m_tableContainerWidget);
    m_filterEdit->setPlaceholderText("Filter files by name or extension...");
    m_filterEdit->setClearButtonEnabled(true);
    tableTopLayout->addWidget(m_filterEdit, 1);

    m_tableCountLabel = new QLabel("0 Files", m_tableContainerWidget);
    m_tableCountLabel->setObjectName("statusPillTotal");
    tableTopLayout->addWidget(m_tableCountLabel);

    tableContainerLayout->addLayout(tableTopLayout);

    // Table
    m_tableView = new QTableView(m_tableContainerWidget);
    m_tableModel = new FileTableModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(FileTableModel::ColOriginalName);

    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setDefaultSectionSize(34);
    m_tableView->verticalHeader()->setVisible(false);

    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColStatus, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColOriginalName, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColNewName, QHeaderView::Stretch);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColExtension, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColSize, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColDate, QHeaderView::ResizeToContents);
    m_tableView->horizontalHeader()->setSectionResizeMode(FileTableModel::ColDestination, QHeaderView::Stretch);

    tableContainerLayout->addWidget(m_tableView);
    m_rightStack->addWidget(m_tableContainerWidget);

    m_rightStack->setCurrentIndex(0); // Start at Empty State
    splitter->addWidget(m_rightStack);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({460, 780});
    rootLayout->addWidget(splitter, 1);

    // =========================================================================
    // 4. BOTTOM ACTION BAR
    // =========================================================================
    auto *bottomCard = createCard(centralWidget);
    auto *bottomLayout = new QHBoxLayout(bottomCard);
    bottomLayout->setContentsMargins(14, 10, 14, 10);
    bottomLayout->setSpacing(10);

    m_summaryTotalPill = new QLabel("0 Files Loaded", bottomCard);
    m_summaryTotalPill->setObjectName("statusPillTotal");

    m_summaryPendingPill = new QLabel("0 Changes Pending", bottomCard);
    m_summaryPendingPill->setObjectName("statusPillPending");

    m_summaryConflictPill = new QLabel("0 Conflicts", bottomCard);
    m_summaryConflictPill->setObjectName("statusPillConflict");
    m_summaryConflictPill->setVisible(false);

    bottomLayout->addWidget(m_summaryTotalPill);
    bottomLayout->addWidget(m_summaryPendingPill);
    bottomLayout->addWidget(m_summaryConflictPill);

    bottomLayout->addStretch();

    m_progressBar = new QProgressBar(bottomCard);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedWidth(220);
    bottomLayout->addWidget(m_progressBar);

    m_applyBtn = new QPushButton("Apply Changes", bottomCard);
    m_applyBtn->setObjectName("primaryButton");
    m_applyBtn->setMinimumWidth(180);
    m_applyBtn->setFixedHeight(38);
    m_applyBtn->setEnabled(false);
    bottomLayout->addWidget(m_applyBtn);

    rootLayout->addWidget(bottomCard);

    // Status bar
    statusBar()->showMessage("Ready. Select a folder or drag files to get started.");

    // Signal connections
    connect(m_selectFolderBtn, &QPushButton::clicked, this, &MainWindow::chooseFolder);
    connect(emptyBrowseBtn, &QPushButton::clicked, this, &MainWindow::chooseFolder);
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
        m_folderPathLabel->setText(dir);
        startScan();
    }
}

void MainWindow::startScan()
{
    if (m_scanner->isRunning()) {
        m_scanner->cancelScan();
        return;
    }

    if (m_currentFolder.isEmpty()) {
        chooseFolder();
        return;
    }

    if (!QDir(m_currentFolder).exists()) {
        QMessageBox::warning(this, "Folder Not Found", "The specified folder does not exist.");
        return;
    }

    m_tableModel->clear();
    m_rightStack->setCurrentIndex(1); // Switch to table view
    m_scanBtn->setEnabled(true);
    m_clearBtn->setEnabled(true);

    m_scanner->setDirectory(m_currentFolder, m_recursiveCheck->isChecked());
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
    m_currentFolder.clear();
    m_folderPathLabel->setText("No folder selected. Choose a folder or drag files into the window.");
    m_rightStack->setCurrentIndex(0); // Switch back to Empty State
    m_scanBtn->setEnabled(false);
    m_clearBtn->setEnabled(false);
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
        RenameOptions opt = m_renamePanel->getOptions();
        RenameEngine::applyRules(items, opt);
        for (FileItem &it : items) {
            it.newDirectoryPath = it.directoryPath;
        }
    } else {
        OrganizeOptions opt = m_organizePanel->getOptions();
        OrganizeEngine::applyOrganization(items, opt, m_currentFolder);
        for (FileItem &it : items) {
            it.newFileName = it.fileName;
        }
    }

    m_lastReport = ConflictDetector::validate(items);
    m_tableModel->updatePreview();
    updateStatusBarCounts();
}

void MainWindow::updateStatusBarCounts()
{
    int total = m_tableModel->rowCount();
    int changed = m_lastReport.changedCount;
    int conflicts = m_lastReport.conflictCount + m_lastReport.existingCollisionCount + m_lastReport.invalidNameCount;

    m_summaryTotalPill->setText(QString("%1 Files Loaded").arg(total));
    m_summaryPendingPill->setText(QString("%1 Changes Pending").arg(changed));
    m_tableCountLabel->setText(QString("%1 Files").arg(total));

    if (conflicts > 0) {
        m_summaryConflictPill->setText(QString("%1 Conflicts Detected").arg(conflicts));
        m_summaryConflictPill->setVisible(true);
        m_applyBtn->setEnabled(false);
    } else {
        m_summaryConflictPill->setVisible(false);
        m_applyBtn->setEnabled(changed > 0);
    }
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

    m_scanBtn->setEnabled(true);
    m_clearBtn->setEnabled(true);
    m_rightStack->setCurrentIndex(1); // Show table

    if (paths.size() == 1 && QFileInfo(paths.first()).isDir()) {
        m_currentFolder = paths.first();
        m_folderPathLabel->setText(m_currentFolder);
        startScan();
    } else {
        m_tableModel->clear();
        m_currentFolder = QFileInfo(paths.first()).absolutePath();
        m_folderPathLabel->setText(m_currentFolder);
        m_scanner->setDirectFiles(paths);
        m_scanner->start();
    }

    event->acceptProposedAction();
}
