#include "OrganizePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>

static QFrame* createCardFrame(QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("cardFrame");
    card->setStyleSheet("QFrame#cardFrame { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; }");
    return card;
}

OrganizePanel::OrganizePanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void OrganizePanel::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: #f8fafc; border: none; } QWidget#orgScrollContainer { background-color: #f8fafc; }");

    auto *container = new QWidget(scrollArea);
    container->setObjectName("orgScrollContainer");
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    // Card 1: Mode Selection
    auto *modeCard = createCardFrame(container);
    auto *modeLayout = new QVBoxLayout(modeCard);
    modeLayout->setContentsMargins(14, 14, 14, 14);
    modeLayout->setSpacing(12);

    auto *modeHeader = new QLabel("Select Organization Method", modeCard);
    modeHeader->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    modeLayout->addWidget(modeHeader);

    m_radioCategory = new QRadioButton("By File Type (Pictures, Documents, Videos, Audio, Archives, Code)", modeCard);
    m_radioCategory->setChecked(true);
    modeLayout->addWidget(m_radioCategory);

    m_radioDate = new QRadioButton("By File Modification Date", modeCard);
    modeLayout->addWidget(m_radioDate);

    auto *dateGroupSub = new QWidget(modeCard);
    auto *dateLayout = new QHBoxLayout(dateGroupSub);
    dateLayout->setContentsMargins(24, 0, 0, 0);
    dateLayout->addWidget(new QLabel("Folder structure:", dateGroupSub));
    m_dateGroupCombo = new QComboBox(dateGroupSub);
    m_dateGroupCombo->addItems({"Year-Month (e.g. 2026-09)", "Year Only (e.g. 2026)", "Year/Month (e.g. 2026/09)"});
    dateLayout->addWidget(m_dateGroupCombo);
    dateLayout->addStretch();
    modeLayout->addWidget(dateGroupSub);

    m_radioSize = new QRadioButton("By File Size (<10 MB, 10-100 MB, 100 MB-1 GB, >1 GB)", modeCard);
    modeLayout->addWidget(m_radioSize);

    m_radioCustom = new QRadioButton("Custom Rule Matching", modeCard);
    modeLayout->addWidget(m_radioCustom);

    layout->addWidget(modeCard);

    // Card 2: Custom Rules
    m_customRuleWidget = createCardFrame(container);
    auto *customLayout = new QVBoxLayout(m_customRuleWidget);
    customLayout->setContentsMargins(14, 14, 14, 14);
    customLayout->setSpacing(10);

    auto *customHeader = new QLabel("Custom Rule Builder", m_customRuleWidget);
    customHeader->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    customLayout->addWidget(customHeader);

    auto *inputRow = new QHBoxLayout();
    m_ruleTypeCombo = new QComboBox(m_customRuleWidget);
    m_ruleTypeCombo->addItems({"Extension is in", "Filename contains", "Filename starts with"});
    m_rulePatternEdit = new QLineEdit(m_customRuleWidget);
    m_rulePatternEdit->setPlaceholderText("e.g. pdf,doc or invoice");
    m_ruleTargetFolderEdit = new QLineEdit(m_customRuleWidget);
    m_ruleTargetFolderEdit->setPlaceholderText("Target folder name");
    auto *addRuleBtn = new QPushButton("Add Rule", m_customRuleWidget);
    addRuleBtn->setObjectName("accentButton");

    inputRow->addWidget(m_ruleTypeCombo);
    inputRow->addWidget(m_rulePatternEdit);
    inputRow->addWidget(m_ruleTargetFolderEdit);
    inputRow->addWidget(addRuleBtn);
    customLayout->addLayout(inputRow);

    m_rulesTable = new QTableWidget(0, 3, m_customRuleWidget);
    m_rulesTable->setHorizontalHeaderLabels({"Condition", "Pattern", "Destination Folder"});
    m_rulesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_rulesTable->setMinimumHeight(140);
    customLayout->addWidget(m_rulesTable);

    auto *ruleActionRow = new QHBoxLayout();
    auto *removeRuleBtn = new QPushButton("Remove Selected Rule", m_customRuleWidget);
    ruleActionRow->addStretch();
    ruleActionRow->addWidget(removeRuleBtn);
    customLayout->addLayout(ruleActionRow);

    layout->addWidget(m_customRuleWidget);
    layout->addStretch();

    scrollArea->setWidget(container);
    rootLayout->addWidget(scrollArea);

    // Connect signals
    connect(m_radioCategory, &QRadioButton::toggled, this, &OrganizePanel::onInputChanged);
    connect(m_radioDate, &QRadioButton::toggled, this, &OrganizePanel::onInputChanged);
    connect(m_radioSize, &QRadioButton::toggled, this, &OrganizePanel::onInputChanged);
    connect(m_radioCustom, &QRadioButton::toggled, this, [this](bool checked) {
        m_customRuleWidget->setEnabled(checked);
        onInputChanged();
    });

    connect(m_dateGroupCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OrganizePanel::onInputChanged);
    connect(addRuleBtn, &QPushButton::clicked, this, &OrganizePanel::addCustomRule);
    connect(removeRuleBtn, &QPushButton::clicked, this, &OrganizePanel::removeSelectedCustomRule);

    m_customRuleWidget->setEnabled(false);
}

void OrganizePanel::addCustomRule()
{
    QString pattern = m_rulePatternEdit->text().trimmed();
    QString target = m_ruleTargetFolderEdit->text().trimmed();
    if (pattern.isEmpty() || target.isEmpty()) {
        return;
    }

    int row = m_rulesTable->rowCount();
    m_rulesTable->insertRow(row);
    m_rulesTable->setItem(row, 0, new QTableWidgetItem(m_ruleTypeCombo->currentText()));
    m_rulesTable->setItem(row, 1, new QTableWidgetItem(pattern));
    m_rulesTable->setItem(row, 2, new QTableWidgetItem(target));

    m_rulePatternEdit->clear();
    m_ruleTargetFolderEdit->clear();

    onInputChanged();
}

void OrganizePanel::removeSelectedCustomRule()
{
    int row = m_rulesTable->currentRow();
    if (row >= 0) {
        m_rulesTable->removeRow(row);
        onInputChanged();
    }
}

void OrganizePanel::onInputChanged()
{
    emit optionsChanged(getOptions());
}

OrganizeOptions OrganizePanel::getOptions() const
{
    OrganizeOptions opt;
    if (m_radioCategory->isChecked()) {
        opt.mode = OrganizeMode::ByCategory;
    } else if (m_radioDate->isChecked()) {
        opt.mode = OrganizeMode::ByDate;
    } else if (m_radioSize->isChecked()) {
        opt.mode = OrganizeMode::BySize;
    } else if (m_radioCustom->isChecked()) {
        opt.mode = OrganizeMode::CustomRule;
    }

    int dateIdx = m_dateGroupCombo->currentIndex();
    if (dateIdx == 0) {
        opt.dateGrouping = DateGrouping::YearMonth;
    } else if (dateIdx == 1) {
        opt.dateGrouping = DateGrouping::Year;
    } else {
        opt.dateGrouping = DateGrouping::YearSlashMonth;
    }

    for (int r = 0; r < m_rulesTable->rowCount(); ++r) {
        CustomOrganizeRule rule;
        QString condText = m_rulesTable->item(r, 0)->text();
        if (condText.contains("Extension")) {
            rule.conditionType = "ext_in";
        } else if (condText.contains("starts")) {
            rule.conditionType = "name_starts";
        } else {
            rule.conditionType = "name_contains";
        }
        rule.pattern = m_rulesTable->item(r, 1)->text();
        rule.targetSubfolder = m_rulesTable->item(r, 2)->text();
        opt.customRules.append(rule);
    }

    return opt;
}
