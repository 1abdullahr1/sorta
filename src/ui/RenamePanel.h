#ifndef RENAMEPANEL_H
#define RENAMEPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "RenameEngine.h"

class RenamePanel : public QWidget {
    Q_OBJECT

public:
    explicit RenamePanel(QWidget *parent = nullptr);

    RenameOptions getOptions() const;

signals:
    void optionsChanged(const RenameOptions &options);

private slots:
    void onInputChanged();
    void resetAllFields();
    void applyQuickPreset(int presetIndex);
    void insertPatternTag(const QString &tag);

private:
    void setupUi();

    // Section 1: Prefix & Suffix
    QCheckBox *m_enablePrefixSuffixCheck;
    QWidget *m_prefixSuffixBody;
    QLineEdit *m_prefixEdit;
    QLineEdit *m_suffixEdit;

    // Section 2: Find & Replace
    QCheckBox *m_enableReplaceCheck;
    QWidget *m_replaceBody;
    QLineEdit *m_findEdit;
    QLineEdit *m_replaceEdit;
    QCheckBox *m_matchCaseCheck;
    QCheckBox *m_regexCheck;

    // Section 3: Sequential Numbering
    QCheckBox *m_enableNumberCheck;
    QWidget *m_numberBody;
    QComboBox *m_numberPosCombo;
    QSpinBox *m_numberStartSpin;
    QSpinBox *m_numberStepSpin;
    QComboBox *m_numberPaddingCombo;
    QLineEdit *m_numberSeparatorEdit;

    // Section 4: Remove Text
    QCheckBox *m_enableRemoveCheck;
    QWidget *m_removeBody;
    QLineEdit *m_removeEdit;

    // Section 5: Case & Timestamps
    QCheckBox *m_enableCaseDateCheck;
    QWidget *m_caseDateBody;
    QComboBox *m_caseCombo;
    QComboBox *m_datePosCombo;
    QComboBox *m_dateSourceCombo;
    QComboBox *m_dateFormatCombo;

    // Section 6: File Extension
    QCheckBox *m_enableExtCheck;
    QWidget *m_extBody;
    QLineEdit *m_newExtEdit;
    QCheckBox *m_lowerExtCheck;
    QCheckBox *m_upperExtCheck;

    // Section 7: Custom Template Pattern
    QCheckBox *m_enablePatternCheck;
    QWidget *m_patternBody;
    QLineEdit *m_patternEdit;
};

#endif // RENAMEPANEL_H
