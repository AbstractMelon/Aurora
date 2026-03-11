#include "commitdialog.h"
#include "git/gitrepository.h"
#include "utils/settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>

CommitDialog::CommitDialog(std::shared_ptr<GitRepository> repo, QWidget *parent)
    : QDialog(parent)
    , m_repository(repo)
{
    setWindowTitle(tr("Commit Changes"));
    setMinimumSize(700, 550);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Message edit
    QLabel *messageLabel = new QLabel(tr("Commit Message:"), this);
    QFont labelFont = messageLabel->font();
    labelFont.setBold(true);
    messageLabel->setFont(labelFont);
    mainLayout->addWidget(messageLabel);
    
    m_messageEdit = new QTextEdit(this);
    m_messageEdit->setPlaceholderText(tr("Summary (required)\n\nOptional extended description..."));
    m_messageEdit->setMaximumHeight(150);
    connect(m_messageEdit, &QTextEdit::textChanged, this, &CommitDialog::onValidate);
    mainLayout->addWidget(m_messageEdit);
    
    // Advanced options
    setupAdvancedOptions();
    mainLayout->addWidget(m_advancedGroup);
    
    // File list
    QLabel *filesLabel = new QLabel(tr("Files to Commit:"), this);
    filesLabel->setFont(labelFont);
    mainLayout->addWidget(filesLabel);
    
    m_fileList = new QListWidget(this);
    mainLayout->addWidget(m_fileList);
    
    loadChangedFiles();
    
    // Button box
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    m_commitButton = new QPushButton(tr("Commit"), this);
    m_commitButton->setEnabled(false);
    m_commitButton->setDefault(true);
    connect(m_commitButton, &QPushButton::clicked, this, &CommitDialog::onCommit);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_commitButton);
    
    mainLayout->addLayout(buttonLayout);
}

void CommitDialog::setupAdvancedOptions()
{
    m_advancedGroup = new QGroupBox(tr("Advanced Options"), this);
    m_advancedGroup->setCheckable(true);
    m_advancedGroup->setChecked(false);
    connect(m_advancedGroup, &QGroupBox::toggled, this, &CommitDialog::onAdvancedToggled);
    
    QVBoxLayout *advLayout = new QVBoxLayout(m_advancedGroup);
    
    // Custom author
    m_customAuthorCheck = new QCheckBox(tr("Override author information"), this);
    advLayout->addWidget(m_customAuthorCheck);
    
    QFormLayout *authorLayout = new QFormLayout();
    m_authorNameEdit = new QLineEdit(this);
    m_authorEmailEdit = new QLineEdit(this);
    
    Settings settings;
    m_authorNameEdit->setText(settings.gitUserName());
    m_authorEmailEdit->setText(settings.gitUserEmail());
    m_authorNameEdit->setEnabled(false);
    m_authorEmailEdit->setEnabled(false);
    
    connect(m_customAuthorCheck, &QCheckBox::toggled, m_authorNameEdit, &QLineEdit::setEnabled);
    connect(m_customAuthorCheck, &QCheckBox::toggled, m_authorEmailEdit, &QLineEdit::setEnabled);
    
    authorLayout->addRow(tr("Name:"), m_authorNameEdit);
    authorLayout->addRow(tr("Email:"), m_authorEmailEdit);
    advLayout->addLayout(authorLayout);
    
    // Custom date
    m_customDateCheck = new QCheckBox(tr("Override commit date"), this);
    advLayout->addWidget(m_customDateCheck);
    
    m_dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    m_dateTimeEdit->setCalendarPopup(true);
    m_dateTimeEdit->setEnabled(false);
    
    connect(m_customDateCheck, &QCheckBox::toggled, m_dateTimeEdit, &QDateTimeEdit::setEnabled);
    
    QFormLayout *dateLayout = new QFormLayout();
    dateLayout->addRow(tr("Date/Time:"), m_dateTimeEdit);
    advLayout->addLayout(dateLayout);
}

QString CommitDialog::getMessage() const
{
    return m_messageEdit->toPlainText();
}

QString CommitDialog::getAuthorName() const
{
    return m_authorNameEdit->text();
}

QString CommitDialog::getAuthorEmail() const
{
    return m_authorEmailEdit->text();
}

QDateTime CommitDialog::getAuthorDate() const
{
    return m_dateTimeEdit->dateTime();
}

bool CommitDialog::useCustomAuthor() const
{
    return m_customAuthorCheck->isChecked();
}

bool CommitDialog::useCustomDate() const
{
    return m_customDateCheck->isChecked();
}

void CommitDialog::loadChangedFiles()
{
    if (!m_repository) return;
    
    auto files = m_repository->status();
    
    for (const auto &file : files) {
        QString statusStr;
        switch (file.status) {
            case 1: statusStr = " [Added]"; break;
            case 2: statusStr = " [Modified]"; break;
            case 3: statusStr = " [Deleted]"; break;
            case 4: statusStr = " [Renamed]"; break;
            case 5: statusStr = " [Untracked]"; break;
            case 6: statusStr = " [Conflicted]"; break;
            default: statusStr = ""; break;
        }
        m_fileList->addItem(file.path + statusStr);
    }
    
    if (files.empty()) {
        m_fileList->addItem(tr("No staged files"));
    }
}

void CommitDialog::onValidate()
{
    bool valid = !m_messageEdit->toPlainText().trimmed().isEmpty();
    m_commitButton->setEnabled(valid);
}

void CommitDialog::onAdvancedToggled(bool checked)
{
    // Just for future use if needed
    Q_UNUSED(checked);
}

void CommitDialog::onCommit()
{
    if (!m_repository) {
        reject();
        return;
    }
    
    QString message = getMessage();
    if (message.trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Invalid Message"),
            tr("Please enter a commit message."));
        return;
    }
    
    try {
        // TODO: Implement custom author/date support in GitRepository
        // For now, just commit with default settings
        m_repository->commit(message);
        accept();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Commit Error"),
            tr("Failed to commit: %1").arg(e.what()));
    }
}
