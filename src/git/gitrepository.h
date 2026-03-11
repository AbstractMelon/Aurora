#ifndef GITREPOSITORY_H
#define GITREPOSITORY_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <memory>
#include <vector>

struct git_repository;
struct git_signature;

struct GitCommit {
    QString oid;
    QString message;
    QString author;
    QString email;
    QDateTime timestamp;
    QStringList parents;
};

struct GitBranch {
    QString name;
    bool isHead;
    bool isRemote;
};

struct GitFileStatus {
    QString path;
    enum Status {
        Unmodified,
        Added,
        Modified,
        Deleted,
        Renamed,
        Untracked,
        Conflicted
    } status;
};

class GitRepository
{
public:
    explicit GitRepository(const QString &path);
    ~GitRepository();
    
    // Static factory methods
    static void clone(const QString &url, const QString &path);
    static void init(const QString &path);
    
    // Repository info
    QString path() const { return m_path; }
    bool isValid() const { return m_repo != nullptr; }
    QString currentBranch() const;
    bool hasUncommittedChanges() const;
    
    // Branch operations
    QStringList branches(bool includeRemote = false) const;
    std::vector<GitBranch> getBranches() const;
    void createBranch(const QString &name, const QString &startPoint = QString());
    void checkoutBranch(const QString &name);
    void deleteBranch(const QString &name);
    
    // Commit operations
    std::vector<GitCommit> log(int maxCount = 100) const;
    void commit(const QString &message, const QStringList &files = QStringList());
    void stageFile(const QString &path);
    void unstageFile(const QString &path);
    void stageAll();
    
    // Status
    std::vector<GitFileStatus> status() const;
    QString diff(const QString &path = QString()) const;
    
    // Remote operations
    void push(const QString &remote = "origin", const QString &branch = QString());
    void pull(const QString &remote = "origin", const QString &branch = QString());
    void fetch(const QString &remote = "origin");
    QStringList remotes() const;
    void addRemote(const QString &name, const QString &url);
    
    // Configuration
    QString getConfig(const QString &key) const;
    void setConfig(const QString &key, const QString &value);
    void configureUser(const QString &name, const QString &email);
    
private:
    git_repository *m_repo;
    QString m_path;
    
    git_signature* getSignature() const;
    void throwIfError(int error, const QString &operation) const;
};

#endif // GITREPOSITORY_H
