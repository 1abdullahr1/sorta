#include "RenamePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

// Helper to create a styled card container
static QFrame* createCardFrame(QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("cardFrame");
    card->setStyleSheet("QFrame#cardFrame { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 8px; }");
    return card;
}

RenamePanel::RenamePanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void RenamePanel::setupUi()
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // Scroll Area for all rename rules
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: #f8fafc; border: none; } QWidget#scrollContainer { background-color: #f8fafc; }");

    auto *container = new QWidget(scrollArea);
    container->setObjectName("scrollContainer");
    auto *layout = new QVBoxLayout(container);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    // --- QUICK ACTION PRESETS BAR ---
    auto *presetCard = createCardFrame(container);
    auto *presetLayout = new QVBoxLayout(presetCard);
    presetLayout->setContentsMargins(12, 10, 12, 10);
    presetLayout->setSpacing(8);

    auto *presetTitle = new QLabel("Quick Presets & Actions", presetCard);
    presetTitle->setStyleSheet("font-weight: 600; color: #475569; font-size: 11.5px; text-transform: uppercase; letter-spacing: 0.5px;");
    presetLayout->addWidget(presetTitle);

    auto *presetBtnRow = new QHBoxLayout();
    presetBtnRow->setSpacing(6);

    auto *btnAutoNum = new QPushButton("Auto Number", presetCard);
    auto *btnFindRep = new QPushButton("Find & Replace", presetCard);
    auto *btnPrefix = new QPushButton("Add Prefix", presetCard);
    auto *btnClear = new QPushButton("Clear Rules", presetCard);
    btnClear->setStyleSheet("color: #ef4444; font-weight: 600;");

    presetBtnRow->addWidget(btnAutoNum);
    presetBtnRow->addWidget(btnFindRep);
    presetBtnRow->addWidget(btnPrefix);
    presetBtnRow->addWidget(btnClear);
    presetLayout->addLayout(presetBtnRow);
    layout->addWidget(presetCard);

    // =========================================================================
    // SECTION 1: FIND & REPLACE
    // =========================================================================
    auto *repCard = createCardFrame(container);
    auto *repLayout = new QVBoxLayout(repCard);
    repLayout->setContentsMargins(14, 12, 14, 14);
    repLayout->setSpacing(10);

    m_enableReplaceCheck = new QCheckBox("Find & Replace Text", repCard);
    m_enableReplaceCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    repLayout->addWidget(m_enableReplaceCheck);

    m_replaceBody = new QWidget(repCard);
    auto *repBodyLayout = new QVBoxLayout(m_replaceBody);
    repBodyLayout->setContentsMargins(0, 4, 0, 0);
    repBodyLayout->setSpacing(8);

    auto *findLabel = new QLabel("Find:", m_replaceBody);
    findLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_findEdit = new QLineEdit(m_replaceBody);
    m_findEdit->setPlaceholderText("Text to find (e.g. IMG_ or Copy)");
    repBodyLayout->addWidget(findLabel);
    repBodyLayout->addWidget(m_findEdit);

    auto *replaceLabel = new QLabel("Replace with:", m_replaceBody);
    replaceLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_replaceEdit = new QLineEdit(m_replaceBody);
    m_replaceEdit->setPlaceholderText("Replacement text (leave empty to delete found text)");
    repBodyLayout->addWidget(replaceLabel);
    repBodyLayout->addWidget(m_replaceEdit);

    auto *repOptionsRow = new QHBoxLayout();
    m_matchCaseCheck = new QCheckBox("Match Case", m_replaceBody);
    m_regexCheck = new QCheckBox("Regular Expression", m_replaceBody);
    repOptionsRow->addWidget(m_matchCaseCheck);
    repOptionsRow->addWidget(m_regexCheck);
    repOptionsRow->addStretch();
    repBodyLayout->addLayout(repOptionsRow);

    m_replaceBody->setVisible(false);
    repLayout->addWidget(m_replaceBody);
    layout->addWidget(repCard);

    // =========================================================================
    // SECTION 2: ADD TEXT (PREFIX & SUFFIX)
    // =========================================================================
    auto *psCard = createCardFrame(container);
    auto *psLayout = new QVBoxLayout(psCard);
    psLayout->setContentsMargins(14, 12, 14, 14);
    psLayout->setSpacing(10);

    m_enablePrefixSuffixCheck = new QCheckBox("Add Prefix & Suffix", psCard);
    m_enablePrefixSuffixCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    psLayout->addWidget(m_enablePrefixSuffixCheck);

    m_prefixSuffixBody = new QWidget(psCard);
    auto *psBodyLayout = new QVBoxLayout(m_prefixSuffixBody);
    psBodyLayout->setContentsMargins(0, 4, 0, 0);
    psBodyLayout->setSpacing(8);

    auto *prefixLabel = new QLabel("Add to Start (Prefix):", m_prefixSuffixBody);
    prefixLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_prefixEdit = new QLineEdit(m_prefixSuffixBody);
    m_prefixEdit->setPlaceholderText("e.g. Vacation_ or 2026_");
    psBodyLayout->addWidget(prefixLabel);
    psBodyLayout->addWidget(m_prefixEdit);

    auto *suffixLabel = new QLabel("Add to End (Suffix):", m_prefixSuffixBody);
    suffixLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_suffixEdit = new QLineEdit(m_prefixSuffixBody);
    m_suffixEdit->setPlaceholderText("e.g. _Edited or _Final");
    psBodyLayout->addWidget(suffixLabel);
    psBodyLayout->addWidget(m_suffixEdit);

    m_prefixSuffixBody->setVisible(false);
    psLayout->addWidget(m_prefixSuffixBody);
    layout->addWidget(psCard);

    // =========================================================================
    // SECTION 3: SEQUENTIAL NUMBERING
    // =========================================================================
    auto *numCard = createCardFrame(container);
    auto *numLayout = new QVBoxLayout(numCard);
    numLayout->setContentsMargins(14, 12, 14, 14);
    numLayout->setSpacing(10);

    m_enableNumberCheck = new QCheckBox("Sequential Numbering", numCard);
    m_enableNumberCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    numLayout->addWidget(m_enableNumberCheck);

    m_numberBody = new QWidget(numCard);
    auto *numBodyLayout = new QVBoxLayout(m_numberBody);
    numBodyLayout->setContentsMargins(0, 4, 0, 0);
    numBodyLayout->setSpacing(8);

    auto *posLabel = new QLabel("Number Position:", m_numberBody);
    posLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_numberPosCombo = new QComboBox(m_numberBody);
    m_numberPosCombo->addItems({"Add at End of Name (Suffix)", "Add at Start of Name (Prefix)", "Replace Entire Filename"});
    numBodyLayout->addWidget(posLabel);
    numBodyLayout->addWidget(m_numberPosCombo);

    auto *numSettingsGrid = new QGridLayout();
    numSettingsGrid->setSpacing(8);

    m_numberStartSpin = new QSpinBox(m_numberBody);
    m_numberStartSpin->setRange(0, 999999);
    m_numberStartSpin->setValue(1);

    m_numberStepSpin = new QSpinBox(m_numberBody);
    m_numberStepSpin->setRange(1, 100);
    m_numberStepSpin->setValue(1);

    m_numberPaddingCombo = new QComboBox(m_numberBody);
    m_numberPaddingCombo->addItems({"001 (3 Digits)", "0001 (4 Digits)", "01 (2 Digits)", "1 (No Padding)"});

    m_numberSeparatorEdit = new QLineEdit("_", m_numberBody);
    m_numberSeparatorEdit->setPlaceholderText("e.g. _ or -");

    numSettingsGrid->addWidget(new QLabel("Start at:", m_numberBody), 0, 0);
    numSettingsGrid->addWidget(m_numberStartSpin, 0, 1);
    numSettingsGrid->addWidget(new QLabel("Step by:", m_numberBody), 0, 2);
    numSettingsGrid->addWidget(m_numberStepSpin, 0, 3);

    numSettingsGrid->addWidget(new QLabel("Padding:", m_numberBody), 1, 0);
    numSettingsGrid->addWidget(m_numberPaddingCombo, 1, 1);
    numSettingsGrid->addWidget(new QLabel("Separator:", m_numberBody), 1, 2);
    numSettingsGrid->addWidget(m_numberSeparatorEdit, 1, 3);

    numBodyLayout->addLayout(numSettingsGrid);

    m_numberBody->setVisible(false);
    numLayout->addWidget(m_numberBody);
    layout->addWidget(numCard);

    // =========================================================================
    // SECTION 4: REMOVE SPECIFIC TEXT
    // =========================================================================
    auto *remCard = createCardFrame(container);
    auto *remLayout = new QVBoxLayout(remCard);
    remLayout->setContentsMargins(14, 12, 14, 14);
    remLayout->setSpacing(10);

    m_enableRemoveCheck = new QCheckBox("Remove Specific Words or Characters", remCard);
    m_enableRemoveCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    remLayout->addWidget(m_enableRemoveCheck);

    m_removeBody = new QWidget(remCard);
    auto *remBodyLayout = new QVBoxLayout(m_removeBody);
    remBodyLayout->setContentsMargins(0, 4, 0, 0);
    remBodyLayout->setSpacing(8);

    auto *removeHint = new QLabel("Remove exact text from filename:", m_removeBody);
    removeHint->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_removeEdit = new QLineEdit(m_removeBody);
    m_removeEdit->setPlaceholderText("e.g. (1) or copy or draft");
    remBodyLayout->addWidget(removeHint);
    remBodyLayout->addWidget(m_removeEdit);

    m_removeBody->setVisible(false);
    remLayout->addWidget(m_removeBody);
    layout->addWidget(remCard);

    // =========================================================================
    // SECTION 5: CAPITALIZATION & DATE STAMPS
    // =========================================================================
    auto *cdCard = createCardFrame(container);
    auto *cdLayout = new QVBoxLayout(cdCard);
    cdLayout->setContentsMargins(14, 12, 14, 14);
    cdLayout->setSpacing(10);

    m_enableCaseDateCheck = new QCheckBox("Change Capitalization & Add Date", cdCard);
    m_enableCaseDateCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    cdLayout->addWidget(m_enableCaseDateCheck);

    m_caseDateBody = new QWidget(cdCard);
    auto *cdBodyLayout = new QVBoxLayout(m_caseDateBody);
    cdBodyLayout->setContentsMargins(0, 4, 0, 0);
    cdBodyLayout->setSpacing(8);

    auto *caseLabel = new QLabel("Letter Case Transformation:", m_caseDateBody);
    caseLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_caseCombo = new QComboBox(m_caseDateBody);
    m_caseCombo->addItems({"No Change", "lowercase (all small)", "UPPERCASE (ALL CAPS)", "Title Case (Capitalize Each Word)", "Sentence case", "camelCase"});
    cdBodyLayout->addWidget(caseLabel);
    cdBodyLayout->addWidget(m_caseCombo);

    auto *dateLabel = new QLabel("Add Date Stamp:", m_caseDateBody);
    dateLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_datePosCombo = new QComboBox(m_caseDateBody);
    m_datePosCombo->addItems({"No Date", "Add at Beginning (Prefix)", "Add at End (Suffix)"});
    cdBodyLayout->addWidget(dateLabel);
    cdBodyLayout->addWidget(m_datePosCombo);

    auto *dateSubGrid = new QGridLayout();
    m_dateSourceCombo = new QComboBox(m_caseDateBody);
    m_dateSourceCombo->addItems({"File Modified Date", "Current Today's Date"});
    m_dateFormatCombo = new QComboBox(m_caseDateBody);
    m_dateFormatCombo->addItems({"yyyy-MM-dd (2026-09-04)", "yyyyMMdd (20260904)", "yyyy-MM-dd_hhmm", "yyyy (2026)"});

    dateSubGrid->addWidget(new QLabel("Source:", m_caseDateBody), 0, 0);
    dateSubGrid->addWidget(m_dateSourceCombo, 0, 1);
    dateSubGrid->addWidget(new QLabel("Format:", m_caseDateBody), 1, 0);
    dateSubGrid->addWidget(m_dateFormatCombo, 1, 1);
    cdBodyLayout->addLayout(dateSubGrid);

    m_caseDateBody->setVisible(false);
    cdLayout->addWidget(m_caseDateBody);
    layout->addWidget(cdCard);

    // =========================================================================
    // SECTION 6: FILE EXTENSION
    // =========================================================================
    auto *extCard = createCardFrame(container);
    auto *extLayout = new QVBoxLayout(extCard);
    extLayout->setContentsMargins(14, 12, 14, 14);
    extLayout->setSpacing(10);

    m_enableExtCheck = new QCheckBox("Modify File Extension", extCard);
    m_enableExtCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    extLayout->addWidget(m_enableExtCheck);

    m_extBody = new QWidget(extCard);
    auto *extBodyLayout = new QVBoxLayout(m_extBody);
    extBodyLayout->setContentsMargins(0, 4, 0, 0);
    extBodyLayout->setSpacing(8);

    auto *newExtLabel = new QLabel("Change Extension to:", m_extBody);
    newExtLabel->setStyleSheet("color: #475569; font-weight: 500; font-size: 12px;");
    m_newExtEdit = new QLineEdit(m_extBody);
    m_newExtEdit->setPlaceholderText("e.g. png or jpg (without dot)");
    extBodyLayout->addWidget(newExtLabel);
    extBodyLayout->addWidget(m_newExtEdit);

    auto *extOptsRow = new QHBoxLayout();
    m_lowerExtCheck = new QCheckBox("Force lowercase (.jpg)", m_extBody);
    m_upperExtCheck = new QCheckBox("Force UPPERCASE (.JPG)", m_extBody);
    extOptsRow->addWidget(m_lowerExtCheck);
    extOptsRow->addWidget(m_upperExtCheck);
    extOptsRow->addStretch();
    extBodyLayout->addLayout(extOptsRow);

    m_extBody->setVisible(false);
    extLayout->addWidget(m_extBody);
    layout->addWidget(extCard);

    // =========================================================================
    // SECTION 7: CUSTOM TEMPLATE PATTERN
    // =========================================================================
    auto *patCard = createCardFrame(container);
    auto *patLayout = new QVBoxLayout(patCard);
    patLayout->setContentsMargins(14, 12, 14, 14);
    patLayout->setSpacing(10);

    m_enablePatternCheck = new QCheckBox("Custom Template Pattern", patCard);
    m_enablePatternCheck->setStyleSheet("font-weight: 600; font-size: 13.5px; color: #0f172a;");
    patLayout->addWidget(m_enablePatternCheck);

    m_patternBody = new QWidget(patCard);
    auto *patBodyLayout = new QVBoxLayout(m_patternBody);
    patBodyLayout->setContentsMargins(0, 4, 0, 0);
    patBodyLayout->setSpacing(8);

    auto *patHelp = new QLabel("Compose a custom naming pattern using tokens:", m_patternBody);
    patHelp->setStyleSheet("color: #475569; font-size: 12px;");
    patBodyLayout->addWidget(patHelp);

    m_patternEdit = new QLineEdit("{name}_{num:3}", m_patternBody);
    m_patternEdit->setPlaceholderText("e.g. {name}_{num:3}_{date}");
    patBodyLayout->addWidget(m_patternEdit);

    auto *tokenRow1 = new QHBoxLayout();
    tokenRow1->setSpacing(6);
    auto *btnName = new QPushButton("+ {name}", m_patternBody);
    btnName->setObjectName("tokenButton");
    auto *btnNum = new QPushButton("+ {num:3}", m_patternBody);
    btnNum->setObjectName("tokenButton");
    auto *btnDate = new QPushButton("+ {date}", m_patternBody);
    btnDate->setObjectName("tokenButton");
    auto *btnParent = new QPushButton("+ {parent}", m_patternBody);
    btnParent->setObjectName("tokenButton");

    tokenRow1->addWidget(btnName);
    tokenRow1->addWidget(btnNum);
    tokenRow1->addWidget(btnDate);
    tokenRow1->addWidget(btnParent);
    tokenRow1->addStretch();
    patBodyLayout->addLayout(tokenRow1);

    m_patternBody->setVisible(false);
    patLayout->addWidget(m_patternBody);
    layout->addWidget(patCard);

    layout->addStretch();
    scrollArea->setWidget(container);
    rootLayout->addWidget(scrollArea);

    // =========================================================================
    // SIGNALS & COLLAPSIBLE EXPANSION
    // =========================================================================
    connect(m_enableReplaceCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_replaceBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_findEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_replaceEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_matchCaseCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_regexCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);

    connect(m_enablePrefixSuffixCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_prefixSuffixBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_prefixEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_suffixEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_enableNumberCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_numberBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_numberPosCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberStartSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberStepSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberPaddingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberSeparatorEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_enableRemoveCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_removeBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_removeEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_enableCaseDateCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_caseDateBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_caseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_datePosCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_dateSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_dateFormatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);

    connect(m_enableExtCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_extBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_newExtEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_lowerExtCheck, &QCheckBox::toggled, this, [this](bool c) {
        if (c) m_upperExtCheck->setChecked(false);
        onInputChanged();
    });
    connect(m_upperExtCheck, &QCheckBox::toggled, this, [this](bool c) {
        if (c) m_lowerExtCheck->setChecked(false);
        onInputChanged();
    });

    connect(m_enablePatternCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_patternBody->setVisible(checked);
        onInputChanged();
    });
    connect(m_patternEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(btnName, &QPushButton::clicked, this, [this]() { insertPatternTag("{name}"); });
    connect(btnNum, &QPushButton::clicked, this, [this]() { insertPatternTag("{num:3}"); });
    connect(btnDate, &QPushButton::clicked, this, [this]() { insertPatternTag("{date}"); });
    connect(btnParent, &QPushButton::clicked, this, [this]() { insertPatternTag("{parent}"); });

    connect(btnClear, &QPushButton::clicked, this, &RenamePanel::resetAllFields);
    connect(btnAutoNum, &QPushButton::clicked, this, [this]() { applyQuickPreset(0); });
    connect(btnFindRep, &QPushButton::clicked, this, [this]() { applyQuickPreset(1); });
    connect(btnPrefix, &QPushButton::clicked, this, [this]() { applyQuickPreset(2); });
}

void RenamePanel::insertPatternTag(const QString &tag)
{
    m_patternEdit->insert(tag);
}

void RenamePanel::applyQuickPreset(int presetIndex)
{
    resetAllFields();
    if (presetIndex == 0) {
        // Auto Number
        m_enableNumberCheck->setChecked(true);
        m_numberPosCombo->setCurrentIndex(0); // Suffix
    } else if (presetIndex == 1) {
        // Find & Replace
        m_enableReplaceCheck->setChecked(true);
        m_findEdit->setFocus();
    } else if (presetIndex == 2) {
        // Prefix
        m_enablePrefixSuffixCheck->setChecked(true);
        m_prefixEdit->setFocus();
    }
}

void RenamePanel::resetAllFields()
{
    m_enableReplaceCheck->setChecked(false);
    m_findEdit->clear();
    m_replaceEdit->clear();
    m_matchCaseCheck->setChecked(false);
    m_regexCheck->setChecked(false);

    m_enablePrefixSuffixCheck->setChecked(false);
    m_prefixEdit->clear();
    m_suffixEdit->clear();

    m_enableNumberCheck->setChecked(false);
    m_numberPosCombo->setCurrentIndex(0);
    m_numberStartSpin->setValue(1);
    m_numberStepSpin->setValue(1);
    m_numberPaddingCombo->setCurrentIndex(0);
    m_numberSeparatorEdit->setText("_");

    m_enableRemoveCheck->setChecked(false);
    m_removeEdit->clear();

    m_enableCaseDateCheck->setChecked(false);
    m_caseCombo->setCurrentIndex(0);
    m_datePosCombo->setCurrentIndex(0);

    m_enableExtCheck->setChecked(false);
    m_newExtEdit->clear();
    m_lowerExtCheck->setChecked(false);
    m_upperExtCheck->setChecked(false);

    m_enablePatternCheck->setChecked(false);
    m_patternEdit->setText("{name}_{num:3}");

    onInputChanged();
}

void RenamePanel::onInputChanged()
{
    emit optionsChanged(getOptions());
}

RenameOptions RenamePanel::getOptions() const
{
    RenameOptions opt;
    opt.usePattern = m_enablePatternCheck->isChecked();
    opt.patternTemplate = m_patternEdit->text();

    if (m_enablePrefixSuffixCheck->isChecked()) {
        opt.prefix = m_prefixEdit->text();
        opt.suffix = m_suffixEdit->text();
    }

    opt.enableReplace = m_enableReplaceCheck->isChecked();
    if (opt.enableReplace) {
        opt.findText = m_findEdit->text();
        opt.replaceText = m_replaceEdit->text();
        opt.matchCase = m_matchCaseCheck->isChecked();
        opt.useRegex = m_regexCheck->isChecked();
    }

    opt.enableRemove = m_enableRemoveCheck->isChecked();
    if (opt.enableRemove) {
        opt.removeText = m_removeEdit->text();
    }

    if (m_enableCaseDateCheck->isChecked()) {
        opt.caseChange = static_cast<CaseConversion>(m_caseCombo->currentIndex());
        opt.datePos = static_cast<DatePosition>(m_datePosCombo->currentIndex());
        opt.dateSource = static_cast<DateSource>(m_dateSourceCombo->currentIndex());
        opt.dateFormat = m_dateFormatCombo->currentText();
    } else {
        opt.caseChange = CaseConversion::None;
        opt.datePos = DatePosition::None;
    }

    opt.enableNumbering = m_enableNumberCheck->isChecked();
    if (opt.enableNumbering) {
        opt.numberPos = static_cast<NumberPosition>(m_numberPosCombo->currentIndex());
        opt.numberStart = m_numberStartSpin->value();
        opt.numberStep = m_numberStepSpin->value();
        int padIdx = m_numberPaddingCombo->currentIndex();
        if (padIdx == 0) opt.numberPadding = 3;
        else if (padIdx == 1) opt.numberPadding = 4;
        else if (padIdx == 2) opt.numberPadding = 2;
        else opt.numberPadding = 1;
        opt.numberSeparator = m_numberSeparatorEdit->text();
    }

    opt.changeExtension = m_enableExtCheck->isChecked();
    if (opt.changeExtension) {
        opt.newExtension = m_newExtEdit->text();
        opt.lowercaseExtension = m_lowerExtCheck->isChecked();
        opt.uppercaseExtension = m_upperExtCheck->isChecked();
    }

    return opt;
}
