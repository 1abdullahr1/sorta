#include "OrganizePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>

OrganizePanel::OrganizePanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void OrganizePanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(12);

    auto *modeGroup = new QGroupBox("Organize & Move Mode", this);
    auto *modeLayout = new QVBoxLayout(modeGroup);

    m_radioCategory = new QRadioButton("By File Category (Images, Documents, Videos, Audio, Archives, Code)", modeGroup);
    m_radioCategory->setChecked(true);

    m_radioDate = new QRadioButton("By Modification Date", modeGroup);
    m_radioSize = new QRadioButton("By File Size (<10MB, 10-100MB, 100MB-1GB, >1GB)", modeGroup);
    m_radioCustom = new QRadioButton("Custom Rule Matching", modeGroup);

    modeLayout->addWidget(m_radioCategory);
    modeLayout->addWidget(m_radioDate);

    // Date grouping options
    auto *dateGroupSub = new QWidget(modeGroup);
    auto *dateLayout = new QHBoxLayout(dateGroupSub);
    dateLayout->setContentsMargins(20, 0, 0, 0);
    dateLayout->addWidget(new QLabel("Grouping format:", dateGroupSub));
    m_dateGroupCombo = new QComboBox(dateGroupSub);
    m_dateGroupCombo->addItems({"Year-Month (e.g. 2026-09)", "Year Only (e.g. 2026)", "Year/Month Subfolders (e.g. 2026/09)"});
    dateLayout->addWidget(m_dateGroupCombo);
    dateLayout->addStretch();
    modeLayout->addWidget(dateGroupSub);

    modeLayout->addWidget(m_radioSize);
    modeLayout->addWidget(m_radioCustom);
    mainLayout->addWidget(modeGroup);

    // Custom Rules Box
    m_customRuleWidget = new QGroupBox("Custom Rules Builder", this);
    auto *customLayout = new QVBoxLayout(m_customRuleWidget);

    auto *inputRow = new QHBoxLayout();
    m_ruleTypeCombo = new QComboBox(m_customRuleWidget);
    m_ruleTypeCombo->addItems({"Extension is in", "Filename contains", "Filename starts with"});
    m_rulePatternEdit = new QLineEdit(m_customRuleWidget);
    m_rulePatternEdit->setPlaceholderText("e.g. pdf,doc or invoice");
    m_ruleTargetFolderEdit = new QLineEdit(m_customRuleWidget);
    m_ruleTargetFolderEdit->setPlaceholderText("Target subfolder name (e.g. Invoices)");
    auto *addRuleBtn = new QPushButton("Add Rule", m_customRuleWidget);

    inputRow->addWidget(new QLabel("If:"));
    inputRow->addWidget(m_ruleTypeCombo);
    inputRow->addWidget(m_rulePatternEdit);
    inputRow->addWidget(new QLabel("Move to:"));
    inputRow->addWidget(m_ruleTargetFolderEdit);
    inputRow->addWidget(addRuleBtn);
    customLayout->addLayout(inputRow);

    m_rulesTable = new QTableWidget(0, 3, m_customRuleWidget);
    m_rulesTable->setHorizontalHeaderLabels({"Condition", "Pattern", "Destination Folder"});
    m_rulesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    customLayout->addWidget(m_rulesTable);

    auto *ruleActionRow = new QHBoxLayout();
    auto *removeRuleBtn = new QPushButton("Remove Selected Rule", m_customRuleWidget);
    ruleActionRow->addStretch();
    ruleActionRow->addWidget(removeRuleBtn);
    customLayout->addLayout(ruleActionRow);

    mainLayout->addWidget(m_customRuleWidget);
    mainLayout->addStretch();

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
