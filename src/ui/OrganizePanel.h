#ifndef ORGANIZEPANEL_H
#define ORGANIZEPANEL_H

#include <QWidget>
#include <QRadioButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include "OrganizeEngine.h"

class OrganizePanel : public QWidget {
    Q_OBJECT

public:
    explicit OrganizePanel(QWidget *parent = nullptr);

    OrganizeOptions getOptions() const;

signals:
    void optionsChanged(const OrganizeOptions &options);

private slots:
    void onInputChanged();
    void addCustomRule();
    void removeSelectedCustomRule();

private:
    void setupUi();

    QRadioButton *m_radioCategory;
    QRadioButton *m_radioDate;
    QRadioButton *m_radioSize;
    QRadioButton *m_radioCustom;

    // Date grouping combo
    QComboBox *m_dateGroupCombo;

    // Custom rule widgets
    QWidget *m_customRuleWidget;
    QTableWidget *m_rulesTable;
    QLineEdit *m_rulePatternEdit;
    QLineEdit *m_ruleTargetFolderEdit;
    QComboBox *m_ruleTypeCombo;
};

#endif // ORGANIZEPANEL_H
