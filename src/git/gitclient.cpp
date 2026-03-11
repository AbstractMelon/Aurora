#include "gitclient.h"
#include <git2.h>
#include <QDebug>

bool GitClient::s_initialized = false;

GitClient::GitClient()
{
    if (!s_initialized) {
        initialize();
    }
}

GitClient::~GitClient()
{
}

void GitClient::initialize()
{
    if (!s_initialized) {
        git_libgit2_init();
        s_initialized = true;
        qDebug() << "libgit2 initialized";
    }
}

void GitClient::shutdown()
{
    if (s_initialized) {
        git_libgit2_shutdown();
        s_initialized = false;
        qDebug() << "libgit2 shutdown";
    }
}

bool GitClient::clone(const QString &url, const QString &path, QString &error)
{
    git_repository *repo = nullptr;
    git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
    
    int result = git_clone(&repo, url.toUtf8().constData(), 
                          path.toUtf8().constData(), &clone_opts);
    
    if (result != 0) {
        error = getLastError();
        return false;
    }
    
    git_repository_free(repo);
    return true;
}

bool GitClient::init(const QString &path, QString &error)
{
    git_repository *repo = nullptr;
    
    int result = git_repository_init(&repo, path.toUtf8().constData(), 0);
    
    if (result != 0) {
        error = getLastError();
        return false;
    }
    
    git_repository_free(repo);
    return true;
}

QString GitClient::getLastError()
{
    const git_error *e = git_error_last();
    if (e) {
        return QString::fromUtf8(e->message);
    }
    return QString("Unknown error");
}
