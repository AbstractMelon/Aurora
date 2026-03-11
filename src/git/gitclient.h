#ifndef GITCLIENT_H
#define GITCLIENT_H

#include <QString>
#include <QStringList>
#include <memory>

// Forward declarations for libgit2 types
struct git_repository;
struct git_commit;
struct git_reference;
struct git_remote;

class GitClient
{
public:
    GitClient();
    ~GitClient();
    
    // Initialize/shutdown libgit2
    static void initialize();
    static void shutdown();
    
    // Repository operations
    static bool clone(const QString &url, const QString &path, QString &error);
    static bool init(const QString &path, QString &error);
    
    // Utility functions
    static QString getLastError();
    
private:
    static bool s_initialized;
};

#endif // GITCLIENT_H
