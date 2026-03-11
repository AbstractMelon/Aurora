#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include "utils/accountmanager.h"

class AccountDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountDialog(QWidget *parent = nullptr);
    
    QString selectedAccount() const;

private slots:
    void onAccountSelected();
    void onAddAccount();
    void onEditAccount();
    void onRemoveAccount();
    void onSetDefault();
    void onSwitchAccount();

private:
    void refreshAccountList();
    void showAccountEditor(const GitAccount &account = GitAccount());
    
    AccountManager m_accountManager;
    QListWidget *m_accountList;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_removeButton;
    QPushButton *m_setDefaultButton;
    QPushButton *m_switchButton;
    QLabel *m_currentLabel;
    
    QString m_selectedAccount;
};

class AccountEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccountEditorDialog(const GitAccount &account, QWidget *parent = nullptr);
    
    GitAccount getAccount() const;

private slots:
    void onValidate();

private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_userNameEdit;
    QLineEdit *m_userEmailEdit;
    QLineEdit *m_githubTokenEdit;
    QLineEdit *m_sshKeyEdit;
    QPushButton *m_browseSshButton;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    
    bool m_isEdit;
};

#endif // ACCOUNTDIALOG_H
