#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>
#include <QSettings>

class Settings
{
public:
    Settings();
    
    // Recent repositories
    QStringList recentRepositories() const;
    void setRecentRepositories(const QStringList &repos);
    void addRecentRepository(const QString &path);
    
    // Git configuration
    QString gitUserName() const;
    void setGitUserName(const QString &name);
    
    QString gitUserEmail() const;
    void setGitUserEmail(const QString &email);
    
    // GitHub
    QString githubToken() const;
    void setGithubToken(const QString &token);
    
    // Editor
    QString externalEditor() const;
    void setExternalEditor(const QString &editor);
    
    // Theme
    QString theme() const;
    void setTheme(const QString &theme);

private:
    QSettings m_settings;
};

#endif // SETTINGS_H
