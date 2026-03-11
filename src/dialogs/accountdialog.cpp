#include "accountdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>

AccountDialog::AccountDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Account Management"));
    setMinimumSize(600, 400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Current account display
    m_currentLabel = new QLabel(this);
    QFont labelFont = m_currentLabel->font();
    labelFont.setBold(true);
    labelFont.setPointSize(11);
    m_currentLabel->setFont(labelFont);
    mainLayout->addWidget(m_currentLabel);
    
    // Account list
    QGroupBox *listGroup = new QGroupBox(tr("Available Accounts"), this);
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);
    
    m_accountList = new QListWidget(this);
    connect(m_accountList, &QListWidget::itemSelectionChanged,
            this, &AccountDialog::onAccountSelected);
    connect(m_accountList, &QListWidget::itemDoubleClicked,
            [this]() { onSwitchAccount(); });
    listLayout->addWidget(m_accountList);
    
    mainLayout->addWidget(listGroup);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton(tr("Add Account"), this);
    connect(m_addButton, &QPushButton::clicked, this, &AccountDialog::onAddAccount);
    
    m_editButton = new QPushButton(tr("Edit"), this);
    m_editButton->setEnabled(false);
    connect(m_editButton, &QPushButton::clicked, this, &AccountDialog::onEditAccount);
    
    m_removeButton = new QPushButton(tr("Remove"), this);
    m_removeButton->setEnabled(false);
    connect(m_removeButton, &QPushButton::clicked, this, &AccountDialog::onRemoveAccount);
    
    m_setDefaultButton = new QPushButton(tr("Set as Default"), this);
    m_setDefaultButton->setEnabled(false);
    connect(m_setDefaultButton, &QPushButton::clicked, this, &AccountDialog::onSetDefault);
    
    m_switchButton = new QPushButton(tr("Switch to Account"), this);
    m_switchButton->setEnabled(false);
    connect(m_switchButton, &QPushButton::clicked, this, &AccountDialog::onSwitchAccount);
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_setDefaultButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_switchButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Close button
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    QPushButton *closeButton = new QPushButton(tr("Close"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    closeLayout->addWidget(closeButton);
    mainLayout->addLayout(closeLayout);
    
    refreshAccountList();
}

QString AccountDialog::selectedAccount() const
{
    return m_selectedAccount;
}

void AccountDialog::refreshAccountList()
{
    m_accountList->clear();
    
    auto accounts = m_accountManager.accounts();
    GitAccount current = m_accountManager.currentAccount();
    
    for (const auto &account : accounts) {
        QString displayText = account.name;
        if (account.name == current.name) {
            displayText += " (Current)";
        }
        if (account.isDefault) {
            displayText += " [Default]";
        }
        
        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, account.name);
        m_accountList->addItem(item);
    }
    
    // Update current account label
    if (current.isValid()) {
        m_currentLabel->setText(tr("Current Account: %1 <%2>")
            .arg(current.userName, current.userEmail));
    } else {
        m_currentLabel->setText(tr("No account selected"));
    }
}

void AccountDialog::onAccountSelected()
{
    bool hasSelection = !m_accountList->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
    m_setDefaultButton->setEnabled(hasSelection);
    m_switchButton->setEnabled(hasSelection);
}

void AccountDialog::onAddAccount()
{
    showAccountEditor();
}

void AccountDialog::onEditAccount()
{
    auto selected = m_accountList->selectedItems();
    if (selected.isEmpty()) return;
    
    QString accountName = selected.first()->data(Qt::UserRole).toString();
    GitAccount account = m_accountManager.getAccount(accountName);
    
    showAccountEditor(account);
}

void AccountDialog::onRemoveAccount()
{
    auto selected = m_accountList->selectedItems();
    if (selected.isEmpty()) return;
    
    QString accountName = selected.first()->data(Qt::UserRole).toString();
    
    auto reply = QMessageBox::question(this, tr("Remove Account"),
        tr("Are you sure you want to remove account '%1'?").arg(accountName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_accountManager.removeAccount(accountName);
        refreshAccountList();
    }
}

void AccountDialog::onSetDefault()
{
    auto selected = m_accountList->selectedItems();
    if (selected.isEmpty()) return;
    
    QString accountName = selected.first()->data(Qt::UserRole).toString();
    m_accountManager.setDefaultAccount(accountName);
    refreshAccountList();
}

void AccountDialog::onSwitchAccount()
{
    auto selected = m_accountList->selectedItems();
    if (selected.isEmpty()) return;
    
    QString accountName = selected.first()->data(Qt::UserRole).toString();
    m_accountManager.setCurrentAccount(accountName);
    m_selectedAccount = accountName;
    refreshAccountList();
    
    QMessageBox::information(this, tr("Account Switched"),
        tr("Switched to account: %1").arg(accountName));
}

void AccountDialog::showAccountEditor(const GitAccount &account)
{
    AccountEditorDialog editor(account, this);
    if (editor.exec() == QDialog::Accepted) {
        GitAccount newAccount = editor.getAccount();
        if (account.name.isEmpty()) {
            m_accountManager.addAccount(newAccount);
        } else {
            m_accountManager.updateAccount(newAccount);
        }
        refreshAccountList();
    }
}

// AccountEditorDialog implementation

AccountEditorDialog::AccountEditorDialog(const GitAccount &account, QWidget *parent)
    : QDialog(parent)
    , m_isEdit(!account.name.isEmpty())
{
    setWindowTitle(m_isEdit ? tr("Edit Account") : tr("Add Account"));
    setMinimumWidth(500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QFormLayout *formLayout = new QFormLayout();
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setText(account.name);
    m_nameEdit->setPlaceholderText(tr("e.g., Work, Personal, GitHub"));
    connect(m_nameEdit, &QLineEdit::textChanged, this, &AccountEditorDialog::onValidate);
    formLayout->addRow(tr("Account Name:"), m_nameEdit);
    
    m_userNameEdit = new QLineEdit(this);
    m_userNameEdit->setText(account.userName);
    m_userNameEdit->setPlaceholderText(tr("Your Name"));
    connect(m_userNameEdit, &QLineEdit::textChanged, this, &AccountEditorDialog::onValidate);
    formLayout->addRow(tr("Git User Name:"), m_userNameEdit);
    
    m_userEmailEdit = new QLineEdit(this);
    m_userEmailEdit->setText(account.userEmail);
    m_userEmailEdit->setPlaceholderText(tr("your.email@example.com"));
    connect(m_userEmailEdit, &QLineEdit::textChanged, this, &AccountEditorDialog::onValidate);
    formLayout->addRow(tr("Git User Email:"), m_userEmailEdit);
    
    m_githubTokenEdit = new QLineEdit(this);
    m_githubTokenEdit->setText(account.githubToken);
    m_githubTokenEdit->setEchoMode(QLineEdit::Password);
    m_githubTokenEdit->setPlaceholderText(tr("ghp_xxxxxxxxxxxxxxxxxxxx (optional)"));
    formLayout->addRow(tr("GitHub Token:"), m_githubTokenEdit);
    
    QHBoxLayout *sshLayout = new QHBoxLayout();
    m_sshKeyEdit = new QLineEdit(this);
    m_sshKeyEdit->setText(account.sshKeyPath);
    m_sshKeyEdit->setPlaceholderText(tr("~/.ssh/id_rsa (optional)"));
    
    m_browseSshButton = new QPushButton(tr("Browse..."), this);
    connect(m_browseSshButton, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(this, tr("Select SSH Key"));
        if (!path.isEmpty()) {
            m_sshKeyEdit->setText(path);
        }
    });
    
    sshLayout->addWidget(m_sshKeyEdit);
    sshLayout->addWidget(m_browseSshButton);
    formLayout->addRow(tr("SSH Key Path:"), sshLayout);
    
    mainLayout->addLayout(formLayout);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    m_saveButton = new QPushButton(tr("Save"), this);
    m_saveButton->setDefault(true);
    m_saveButton->setEnabled(false);
    connect(m_saveButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_saveButton);
    
    mainLayout->addLayout(buttonLayout);
    
    onValidate();
}

GitAccount AccountEditorDialog::getAccount() const
{
    GitAccount account;
    account.name = m_nameEdit->text().trimmed();
    account.userName = m_userNameEdit->text().trimmed();
    account.userEmail = m_userEmailEdit->text().trimmed();
    account.githubToken = m_githubTokenEdit->text().trimmed();
    account.sshKeyPath = m_sshKeyEdit->text().trimmed();
    account.isDefault = false;
    return account;
}

void AccountEditorDialog::onValidate()
{
    bool valid = !m_nameEdit->text().trimmed().isEmpty() &&
                 !m_userNameEdit->text().trimmed().isEmpty() &&
                 !m_userEmailEdit->text().trimmed().isEmpty();
    
    m_saveButton->setEnabled(valid);
}
