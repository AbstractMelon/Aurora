#include "settingsdialog.h"
#include "utils/settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QWidget>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(500, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget(this);
    
    setupGeneralTab();
    setupGitTab();
    setupGitHubTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // Button box
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancel);
    
    m_saveButton = new QPushButton(tr("Save"), this);
    m_saveButton->setDefault(true);
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::onSave);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_saveButton);
    
    mainLayout->addLayout(buttonLayout);
    
    loadSettings();
}

void SettingsDialog::setupGeneralTab()
{
    QWidget *generalTab = new QWidget();
    QFormLayout *layout = new QFormLayout(generalTab);
    
    m_editorEdit = new QLineEdit();
    m_editorEdit->setPlaceholderText(tr("code, vim, nano, etc."));
    layout->addRow(tr("External Editor:"), m_editorEdit);
    
    m_tabWidget->addTab(generalTab, tr("General"));
}

void SettingsDialog::setupGitTab()
{
    QWidget *gitTab = new QWidget();
    QFormLayout *layout = new QFormLayout(gitTab);
    
    m_userNameEdit = new QLineEdit();
    m_userNameEdit->setPlaceholderText(tr("Your Name"));
    layout->addRow(tr("User Name:"), m_userNameEdit);
    
    m_userEmailEdit = new QLineEdit();
    m_userEmailEdit->setPlaceholderText(tr("your.email@example.com"));
    layout->addRow(tr("User Email:"), m_userEmailEdit);
    
    m_tabWidget->addTab(gitTab, tr("Git"));
}

void SettingsDialog::setupGitHubTab()
{
    QWidget *githubTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(githubTab);
    
    QLabel *info = new QLabel(tr("Enter your GitHub Personal Access Token for API access:"));
    info->setWordWrap(true);
    layout->addWidget(info);
    
    QFormLayout *formLayout = new QFormLayout();
    m_githubTokenEdit = new QLineEdit();
    m_githubTokenEdit->setEchoMode(QLineEdit::Password);
    m_githubTokenEdit->setPlaceholderText(tr("ghp_xxxxxxxxxxxxxxxxxxxx"));
    formLayout->addRow(tr("Access Token:"), m_githubTokenEdit);
    
    layout->addLayout(formLayout);
    layout->addStretch();
    
    m_tabWidget->addTab(githubTab, tr("GitHub"));
}

void SettingsDialog::loadSettings()
{
    Settings settings;
    
    m_editorEdit->setText(settings.externalEditor());
    m_userNameEdit->setText(settings.gitUserName());
    m_userEmailEdit->setText(settings.gitUserEmail());
    m_githubTokenEdit->setText(settings.githubToken());
}

void SettingsDialog::saveSettings()
{
    Settings settings;
    
    settings.setExternalEditor(m_editorEdit->text());
    settings.setGitUserName(m_userNameEdit->text());
    settings.setGitUserEmail(m_userEmailEdit->text());
    settings.setGithubToken(m_githubTokenEdit->text());
}

void SettingsDialog::onSave()
{
    saveSettings();
    accept();
}

void SettingsDialog::onCancel()
{
    reject();
}
