#ifndef RENAMEPANEL_H
#define RENAMEPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
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
    void insertPatternTag(const QString &tag);

private:
    void setupUi();

    // Mode selection
    QRadioButton *m_radioStandardMode;
    QRadioButton *m_radioPatternMode;

    // Pattern mode controls
    QWidget *m_patternWidget;
    QLineEdit *m_patternEdit;

    // Standard mode controls
    QWidget *m_standardWidget;

    // Prefix & Suffix
    QLineEdit *m_prefixEdit;
    QLineEdit *m_suffixEdit;

    // Find & Replace
    QCheckBox *m_enableReplaceCheck;
    QLineEdit *m_findEdit;
    QLineEdit *m_replaceEdit;
    QCheckBox *m_matchCaseCheck;
    QCheckBox *m_regexCheck;

    // Remove text
    QCheckBox *m_enableRemoveCheck;
    QLineEdit *m_removeEdit;

    // Case change
    QComboBox *m_caseCombo;

    // Numbering
    QCheckBox *m_enableNumberCheck;
    QComboBox *m_numberPosCombo;
    QSpinBox *m_numberStartSpin;
    QSpinBox *m_numberStepSpin;
    QSpinBox *m_numberPaddingSpin;
    QLineEdit *m_numberSeparatorEdit;

    // Date
    QComboBox *m_datePosCombo;
    QComboBox *m_dateSourceCombo;
    QComboBox *m_dateFormatCombo;

    // Extension
    QCheckBox *m_changeExtCheck;
    QLineEdit *m_newExtEdit;
    QCheckBox *m_lowerExtCheck;
    QCheckBox *m_upperExtCheck;
};

#endif // RENAMEPANEL_H
