#include "accountmanager.h"

AccountManager::AccountManager()
    : m_settings("AbstractMelon", "Aurora")
{
    loadAccounts();
}

QList<GitAccount> AccountManager::accounts() const
{
    return m_accounts;
}

GitAccount AccountManager::currentAccount() const
{
    if (m_currentAccount.isEmpty() && !m_accounts.isEmpty()) {
        return m_accounts.first();
    }
    
    return getAccount(m_currentAccount);
}

GitAccount AccountManager::defaultAccount() const
{
    for (const auto &account : m_accounts) {
        if (account.isDefault) {
            return account;
        }
    }
    
    if (!m_accounts.isEmpty()) {
        return m_accounts.first();
    }
    
    return GitAccount();
}

GitAccount AccountManager::getAccount(const QString &name) const
{
    for (const auto &account : m_accounts) {
        if (account.name == name) {
            return account;
        }
    }
    
    return GitAccount();
}

void AccountManager::addAccount(const GitAccount &account)
{
    if (!account.isValid()) return;
    
    // Remove existing account with same name
    removeAccount(account.name);
    
    m_accounts.append(account);
    saveAccounts();
    
    // Set as current if it's the first account
    if (m_accounts.size() == 1) {
        setCurrentAccount(account.name);
    }
}

void AccountManager::updateAccount(const GitAccount &account)
{
    for (int i = 0; i < m_accounts.size(); ++i) {
        if (m_accounts[i].name == account.name) {
            m_accounts[i] = account;
            saveAccounts();
            return;
        }
    }
}

void AccountManager::removeAccount(const QString &name)
{
    for (int i = 0; i < m_accounts.size(); ++i) {
        if (m_accounts[i].name == name) {
            m_accounts.removeAt(i);
            saveAccounts();
            
            // If this was the current account, switch to first available
            if (m_currentAccount == name && !m_accounts.isEmpty()) {
                setCurrentAccount(m_accounts.first().name);
            }
            return;
        }
    }
}

void AccountManager::setCurrentAccount(const QString &name)
{
    if (getAccount(name).isValid()) {
        m_currentAccount = name;
        m_settings.setValue("accounts/current", name);
    }
}

void AccountManager::setDefaultAccount(const QString &name)
{
    for (int i = 0; i < m_accounts.size(); ++i) {
        m_accounts[i].isDefault = (m_accounts[i].name == name);
    }
    saveAccounts();
}

QString AccountManager::currentUserName() const
{
    return currentAccount().userName;
}

QString AccountManager::currentUserEmail() const
{
    return currentAccount().userEmail;
}

QString AccountManager::currentGitHubToken() const
{
    return currentAccount().githubToken;
}

bool AccountManager::hasAccounts() const
{
    return !m_accounts.isEmpty();
}

void AccountManager::loadAccounts()
{
    m_accounts.clear();
    
    int count = m_settings.beginReadArray("accounts/list");
    for (int i = 0; i < count; ++i) {
        m_settings.setArrayIndex(i);
        
        GitAccount account;
        account.name = m_settings.value("name").toString();
        account.userName = m_settings.value("userName").toString();
        account.userEmail = m_settings.value("userEmail").toString();
        account.githubToken = m_settings.value("githubToken").toString();
        account.sshKeyPath = m_settings.value("sshKeyPath").toString();
        account.isDefault = m_settings.value("isDefault", false).toBool();
        
        if (account.isValid()) {
            m_accounts.append(account);
        }
    }
    m_settings.endArray();
    
    m_currentAccount = m_settings.value("accounts/current").toString();
}

void AccountManager::saveAccounts()
{
    m_settings.beginWriteArray("accounts/list");
    for (int i = 0; i < m_accounts.size(); ++i) {
        m_settings.setArrayIndex(i);
        m_settings.setValue("name", m_accounts[i].name);
        m_settings.setValue("userName", m_accounts[i].userName);
        m_settings.setValue("userEmail", m_accounts[i].userEmail);
        m_settings.setValue("githubToken", m_accounts[i].githubToken);
        m_settings.setValue("sshKeyPath", m_accounts[i].sshKeyPath);
        m_settings.setValue("isDefault", m_accounts[i].isDefault);
    }
    m_settings.endArray();
}

QString AccountManager::accountKey(const QString &name) const
{
    return QString("accounts/%1").arg(name);
}
