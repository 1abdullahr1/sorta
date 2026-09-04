#include "AboutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About Sorta");
    setFixedSize(460, 320);
    setupUi();
}

void AboutDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 20);
    mainLayout->setSpacing(16);

    auto *headerLayout = new QHBoxLayout();
    auto *iconLabel = new QLabel(this);
    QPixmap icon(":/app.png");
    if (!icon.isNull()) {
        iconLabel->setPixmap(icon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    headerLayout->addWidget(iconLabel);

    auto *titleLayout = new QVBoxLayout();
    auto *titleLabel = new QLabel("Sorta", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #0f172a;");
    auto *subtitleLabel = new QLabel("High-Speed Bulk File Renamer & Organizer", this);
    subtitleLabel->setStyleSheet("font-size: 13px; color: #2563eb; font-weight: 600;");
    auto *versionLabel = new QLabel("Version 1.0.0 (Windows Native x64)", this);
    versionLabel->setStyleSheet("font-size: 12px; color: #64748b;");

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);
    titleLayout->addWidget(versionLabel);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    mainLayout->addLayout(headerLayout);

    auto *descLabel = new QLabel(
        "Sorta is a native desktop utility designed to organize and rename thousands of files "
        "in milliseconds. Built with modern C++17 and Qt 6 for maximum speed, memory efficiency, "
        "and complete filesystem safety.", this);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #334155; line-height: 1.4; font-size: 12.5px;");
    mainLayout->addWidget(descLabel);

    auto *creditsLabel = new QLabel("Crafted by <b>Abdullah Bhatti</b><br>Licensed under the MIT License.", this);
    creditsLabel->setStyleSheet("color: #64748b; font-size: 12px;");
    mainLayout->addWidget(creditsLabel);

    mainLayout->addStretch();

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto *okBtn = new QPushButton("Close", this);
    okBtn->setFixedWidth(100);
    btnLayout->addWidget(okBtn);
    mainLayout->addLayout(btnLayout);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
}
