#include "settings.h"

Settings::Settings()
    : m_settings("AbstractMelon", "Aurora")
{
}

QStringList Settings::recentRepositories() const
{
    return m_settings.value("repositories/recent").toStringList();
}

void Settings::setRecentRepositories(const QStringList &repos)
{
    m_settings.setValue("repositories/recent", repos);
}

void Settings::addRecentRepository(const QString &path)
{
    QStringList repos = recentRepositories();
    repos.removeAll(path);
    repos.prepend(path);
    
    // Keep only the last 10
    while (repos.size() > 10) {
        repos.removeLast();
    }
    
    setRecentRepositories(repos);
}

QString Settings::gitUserName() const
{
    return m_settings.value("git/userName").toString();
}

void Settings::setGitUserName(const QString &name)
{
    m_settings.setValue("git/userName", name);
}

QString Settings::gitUserEmail() const
{
    return m_settings.value("git/userEmail").toString();
}

void Settings::setGitUserEmail(const QString &email)
{
    m_settings.setValue("git/userEmail", email);
}

QString Settings::githubToken() const
{
    return m_settings.value("github/token").toString();
}

void Settings::setGithubToken(const QString &token)
{
    m_settings.setValue("github/token", token);
}

QString Settings::externalEditor() const
{
    return m_settings.value("editor/external", "code").toString();
}

void Settings::setExternalEditor(const QString &editor)
{
    m_settings.setValue("editor/external", editor);
}

QString Settings::theme() const
{
    return m_settings.value("ui/theme", "dark").toString();
}

void Settings::setTheme(const QString &theme)
{
    m_settings.setValue("ui/theme", theme);
}
