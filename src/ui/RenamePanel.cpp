#include "RenamePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>

RenamePanel::RenamePanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void RenamePanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(8);

    // Scroll Area to fit all options nicely
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget(scrollArea);
    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(6, 6, 6, 6);
    containerLayout->setSpacing(12);

    // Mode Selection Bar
    auto *modeLayout = new QHBoxLayout();
    m_radioStandardMode = new QRadioButton("Rules Builder", container);
    m_radioPatternMode = new QRadioButton("Template Pattern Mode", container);
    m_radioStandardMode->setChecked(true);

    auto *modeGroup = new QButtonGroup(container);
    modeGroup->addButton(m_radioStandardMode);
    modeGroup->addButton(m_radioPatternMode);

    auto *resetBtn = new QPushButton("Reset Rules", container);
    resetBtn->setMaximumWidth(120);

    modeLayout->addWidget(m_radioStandardMode);
    modeLayout->addWidget(m_radioPatternMode);
    modeLayout->addStretch();
    modeLayout->addWidget(resetBtn);
    containerLayout->addLayout(modeLayout);

    // --- TEMPLATE PATTERN WIDGET ---
    m_patternWidget = new QWidget(container);
    auto *patLayout = new QVBoxLayout(m_patternWidget);
    patLayout->setContentsMargins(0, 0, 0, 0);

    auto *patGroup = new QGroupBox("Custom Naming Pattern", m_patternWidget);
    auto *patGroupLayout = new QVBoxLayout(patGroup);

    m_patternEdit = new QLineEdit("{name}_{num:3}", patGroup);
    m_patternEdit->setPlaceholderText("e.g. {name}_{num:3}_{date}");
    patGroupLayout->addWidget(new QLabel("Type custom template using available tokens:", patGroup));
    patGroupLayout->addWidget(m_patternEdit);

    auto *tokensLayout = new QHBoxLayout();
    auto *btnTagOriginal = new QPushButton("{name}", patGroup);
    auto *btnTagNum = new QPushButton("{num:3}", patGroup);
    auto *btnTagDate = new QPushButton("{date}", patGroup);
    auto *btnTagTime = new QPushButton("{time}", patGroup);
    auto *btnTagParent = new QPushButton("{parent}", patGroup);
    auto *btnTagSize = new QPushButton("{size}", patGroup);

    tokensLayout->addWidget(btnTagOriginal);
    tokensLayout->addWidget(btnTagNum);
    tokensLayout->addWidget(btnTagDate);
    tokensLayout->addWidget(btnTagTime);
    tokensLayout->addWidget(btnTagParent);
    tokensLayout->addWidget(btnTagSize);
    tokensLayout->addStretch();
    patGroupLayout->addLayout(tokensLayout);

    patLayout->addWidget(patGroup);
    m_patternWidget->setVisible(false); // Initially hidden
    containerLayout->addWidget(m_patternWidget);

    // --- STANDARD RULES WIDGET ---
    m_standardWidget = new QWidget(container);
    auto *stdLayout = new QVBoxLayout(m_standardWidget);
    stdLayout->setContentsMargins(0, 0, 0, 0);
    stdLayout->setSpacing(12);

    // 1. Text Addition (Prefix / Suffix)
    auto *textGroup = new QGroupBox("Text Addition", m_standardWidget);
    auto *textGrid = new QGridLayout(textGroup);
    m_prefixEdit = new QLineEdit(textGroup);
    m_prefixEdit->setPlaceholderText("Prefix (e.g. Vacation_)");
    m_suffixEdit = new QLineEdit(textGroup);
    m_suffixEdit->setPlaceholderText("Suffix (e.g. _Final)");

    textGrid->addWidget(new QLabel("Prefix:"), 0, 0);
    textGrid->addWidget(m_prefixEdit, 0, 1);
    textGrid->addWidget(new QLabel("Suffix:"), 0, 2);
    textGrid->addWidget(m_suffixEdit, 0, 3);
    stdLayout->addWidget(textGroup);

    // 2. Find & Replace
    auto *replaceGroup = new QGroupBox("Find & Replace", m_standardWidget);
    auto *repGrid = new QGridLayout(replaceGroup);
    m_enableReplaceCheck = new QCheckBox("Enable Find & Replace", replaceGroup);
    m_findEdit = new QLineEdit(replaceGroup);
    m_findEdit->setPlaceholderText("Find text...");
    m_replaceEdit = new QLineEdit(replaceGroup);
    m_replaceEdit->setPlaceholderText("Replace with...");
    m_matchCaseCheck = new QCheckBox("Match Case", replaceGroup);
    m_regexCheck = new QCheckBox("Use Regular Expression", replaceGroup);

    repGrid->addWidget(m_enableReplaceCheck, 0, 0, 1, 4);
    repGrid->addWidget(new QLabel("Find:"), 1, 0);
    repGrid->addWidget(m_findEdit, 1, 1);
    repGrid->addWidget(new QLabel("Replace:"), 1, 2);
    repGrid->addWidget(m_replaceEdit, 1, 3);
    repGrid->addWidget(m_matchCaseCheck, 2, 1);
    repGrid->addWidget(m_regexCheck, 2, 3);
    stdLayout->addWidget(replaceGroup);

    // 3. Remove Text
    auto *removeGroup = new QGroupBox("Remove Text", m_standardWidget);
    auto *removeLayout = new QHBoxLayout(removeGroup);
    m_enableRemoveCheck = new QCheckBox("Remove Specific Text:", removeGroup);
    m_removeEdit = new QLineEdit(removeGroup);
    m_removeEdit->setPlaceholderText("Characters or words to strip out");
    removeLayout->addWidget(m_enableRemoveCheck);
    removeLayout->addWidget(m_removeEdit);
    stdLayout->addWidget(removeGroup);

    // 4. Sequential Numbering
    auto *numGroup = new QGroupBox("Sequential Numbering", m_standardWidget);
    auto *numGrid = new QGridLayout(numGroup);
    m_enableNumberCheck = new QCheckBox("Add Sequential Numbers", numGroup);

    m_numberPosCombo = new QComboBox(numGroup);
    m_numberPosCombo->addItems({"Suffix (End of name)", "Prefix (Beginning of name)", "Replace entire name"});

    m_numberStartSpin = new QSpinBox(numGroup);
    m_numberStartSpin->setRange(0, 999999);
    m_numberStartSpin->setValue(1);

    m_numberStepSpin = new QSpinBox(numGroup);
    m_numberStepSpin->setRange(1, 100);
    m_numberStepSpin->setValue(1);

    m_numberPaddingSpin = new QSpinBox(numGroup);
    m_numberPaddingSpin->setRange(1, 10);
    m_numberPaddingSpin->setValue(3);

    m_numberSeparatorEdit = new QLineEdit("_", numGroup);
    m_numberSeparatorEdit->setMaximumWidth(60);

    numGrid->addWidget(m_enableNumberCheck, 0, 0, 1, 4);
    numGrid->addWidget(new QLabel("Placement:"), 1, 0);
    numGrid->addWidget(m_numberPosCombo, 1, 1);
    numGrid->addWidget(new QLabel("Separator:"), 1, 2);
    numGrid->addWidget(m_numberSeparatorEdit, 1, 3);

    numGrid->addWidget(new QLabel("Start Number:"), 2, 0);
    numGrid->addWidget(m_numberStartSpin, 2, 1);
    numGrid->addWidget(new QLabel("Padding Digits (001):"), 2, 2);
    numGrid->addWidget(m_numberPaddingSpin, 2, 3);

    stdLayout->addWidget(numGroup);

    // 5. Case Transformation & Date Stamp
    auto *caseDateGroup = new QGroupBox("Capitalization & Timestamps", m_standardWidget);
    auto *cdGrid = new QGridLayout(caseDateGroup);

    m_caseCombo = new QComboBox(caseDateGroup);
    m_caseCombo->addItems({"No Change", "lowercase", "UPPERCASE", "Title Case", "Sentence case", "camelCase"});

    m_datePosCombo = new QComboBox(caseDateGroup);
    m_datePosCombo->addItems({"No Date", "Add Date as Prefix", "Add Date as Suffix"});

    m_dateSourceCombo = new QComboBox(caseDateGroup);
    m_dateSourceCombo->addItems({"Modified Date", "Current Date"});

    m_dateFormatCombo = new QComboBox(caseDateGroup);
    m_dateFormatCombo->addItems({"yyyy-MM-dd", "yyyyMMdd", "yyyy-MM-dd_hhmm", "yyyy"});

    cdGrid->addWidget(new QLabel("Change Case:"), 0, 0);
    cdGrid->addWidget(m_caseCombo, 0, 1);
    cdGrid->addWidget(new QLabel("Add Date:"), 0, 2);
    cdGrid->addWidget(m_datePosCombo, 0, 3);

    cdGrid->addWidget(new QLabel("Date Source:"), 1, 2);
    cdGrid->addWidget(m_dateSourceCombo, 1, 3);
    cdGrid->addWidget(new QLabel("Date Format:"), 2, 2);
    cdGrid->addWidget(m_dateFormatCombo, 2, 3);

    stdLayout->addWidget(caseDateGroup);

    // 6. Extension Modification
    auto *extGroup = new QGroupBox("File Extension", m_standardWidget);
    auto *extGrid = new QGridLayout(extGroup);
    m_changeExtCheck = new QCheckBox("Change Extension:", extGroup);
    m_newExtEdit = new QLineEdit(extGroup);
    m_newExtEdit->setPlaceholderText("e.g. png");
    m_lowerExtCheck = new QCheckBox("Force lowercase (e.g. .jpg)", extGroup);
    m_upperExtCheck = new QCheckBox("Force UPPERCASE (e.g. .JPG)", extGroup);

    extGrid->addWidget(m_changeExtCheck, 0, 0);
    extGrid->addWidget(m_newExtEdit, 0, 1);
    extGrid->addWidget(m_lowerExtCheck, 1, 0);
    extGrid->addWidget(m_upperExtCheck, 1, 1);
    stdLayout->addWidget(extGroup);

    containerLayout->addWidget(m_standardWidget);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    // Signal connections for real-time reactivity
    connect(m_radioStandardMode, &QRadioButton::toggled, this, [this](bool checked) {
        m_standardWidget->setVisible(checked);
        m_patternWidget->setVisible(!checked);
        onInputChanged();
    });

    connect(m_patternEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_prefixEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_suffixEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_enableReplaceCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_findEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_replaceEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_matchCaseCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_regexCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);

    connect(m_enableRemoveCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_removeEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_caseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);

    connect(m_enableNumberCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_numberPosCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberStartSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberStepSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberPaddingSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &RenamePanel::onInputChanged);
    connect(m_numberSeparatorEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);

    connect(m_datePosCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_dateSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);
    connect(m_dateFormatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RenamePanel::onInputChanged);

    connect(m_changeExtCheck, &QCheckBox::toggled, this, &RenamePanel::onInputChanged);
    connect(m_newExtEdit, &QLineEdit::textChanged, this, &RenamePanel::onInputChanged);
    connect(m_lowerExtCheck, &QCheckBox::toggled, this, [this](bool c) {
        if (c) m_upperExtCheck->setChecked(false);
        onInputChanged();
    });
    connect(m_upperExtCheck, &QCheckBox::toggled, this, [this](bool c) {
        if (c) m_lowerExtCheck->setChecked(false);
        onInputChanged();
    });

    connect(resetBtn, &QPushButton::clicked, this, &RenamePanel::resetAllFields);

    connect(btnTagOriginal, &QPushButton::clicked, this, [this]() { insertPatternTag("{name}"); });
    connect(btnTagNum, &QPushButton::clicked, this, [this]() { insertPatternTag("{num:3}"); });
    connect(btnTagDate, &QPushButton::clicked, this, [this]() { insertPatternTag("{date}"); });
    connect(btnTagTime, &QPushButton::clicked, this, [this]() { insertPatternTag("{time}"); });
    connect(btnTagParent, &QPushButton::clicked, this, [this]() { insertPatternTag("{parent}"); });
    connect(btnTagSize, &QPushButton::clicked, this, [this]() { insertPatternTag("{size}"); });
}

void RenamePanel::insertPatternTag(const QString &tag)
{
    m_patternEdit->insert(tag);
}

void RenamePanel::resetAllFields()
{
    m_prefixEdit->clear();
    m_suffixEdit->clear();
    m_enableReplaceCheck->setChecked(false);
    m_findEdit->clear();
    m_replaceEdit->clear();
    m_enableRemoveCheck->setChecked(false);
    m_removeEdit->clear();
    m_caseCombo->setCurrentIndex(0);
    m_enableNumberCheck->setChecked(false);
    m_numberPosCombo->setCurrentIndex(0);
    m_numberStartSpin->setValue(1);
    m_numberPaddingSpin->setValue(3);
    m_datePosCombo->setCurrentIndex(0);
    m_changeExtCheck->setChecked(false);
    m_newExtEdit->clear();
    m_lowerExtCheck->setChecked(false);
    m_upperExtCheck->setChecked(false);
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
    opt.usePattern = m_radioPatternMode->isChecked();
    opt.patternTemplate = m_patternEdit->text();

    opt.prefix = m_prefixEdit->text();
    opt.suffix = m_suffixEdit->text();

    opt.enableReplace = m_enableReplaceCheck->isChecked();
    opt.findText = m_findEdit->text();
    opt.replaceText = m_replaceEdit->text();
    opt.matchCase = m_matchCaseCheck->isChecked();
    opt.useRegex = m_regexCheck->isChecked();

    opt.enableRemove = m_enableRemoveCheck->isChecked();
    opt.removeText = m_removeEdit->text();

    opt.caseChange = static_cast<CaseConversion>(m_caseCombo->currentIndex());

    opt.enableNumbering = m_enableNumberCheck->isChecked();
    opt.numberPos = static_cast<NumberPosition>(m_numberPosCombo->currentIndex());
    opt.numberStart = m_numberStartSpin->value();
    opt.numberStep = m_numberStepSpin->value();
    opt.numberPadding = m_numberPaddingSpin->value();
    opt.numberSeparator = m_numberSeparatorEdit->text();

    opt.datePos = static_cast<DatePosition>(m_datePosCombo->currentIndex());
    opt.dateSource = static_cast<DateSource>(m_dateSourceCombo->currentIndex());
    opt.dateFormat = m_dateFormatCombo->currentText();

    opt.changeExtension = m_changeExtCheck->isChecked();
    opt.newExtension = m_newExtEdit->text();
    opt.lowercaseExtension = m_lowerExtCheck->isChecked();
    opt.uppercaseExtension = m_upperExtCheck->isChecked();

    return opt;
}
