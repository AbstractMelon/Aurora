#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QList>

struct GitAccount {
    QString name;           // Account display name
    QString userName;       // Git user.name
    QString userEmail;      // Git user.email
    QString githubToken;    // GitHub personal access token
    QString sshKeyPath;     // Path to SSH key (optional)
    bool isDefault;         // Is this the default account
    
    bool isValid() const {
        return !name.isEmpty() && !userName.isEmpty() && !userEmail.isEmpty();
    }
};

class AccountManager
{
public:
    AccountManager();
    
    // Account management
    QList<GitAccount> accounts() const;
    GitAccount currentAccount() const;
    GitAccount defaultAccount() const;
    GitAccount getAccount(const QString &name) const;
    
    void addAccount(const GitAccount &account);
    void updateAccount(const GitAccount &account);
    void removeAccount(const QString &name);
    void setCurrentAccount(const QString &name);
    void setDefaultAccount(const QString &name);
    
    // Quick access
    QString currentUserName() const;
    QString currentUserEmail() const;
    QString currentGitHubToken() const;
    
    bool hasAccounts() const;
    
private:
    void loadAccounts();
    void saveAccounts();
    QString accountKey(const QString &name) const;
    
    QSettings m_settings;
    QList<GitAccount> m_accounts;
    QString m_currentAccount;
};

#endif // ACCOUNTMANAGER_H
