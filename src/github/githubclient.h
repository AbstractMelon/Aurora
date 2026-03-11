#ifndef GITHUBCLIENT_H
#define GITHUBCLIENT_H

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct GitHubRepository {
    QString name;
    QString fullName;
    QString description;
    QString cloneUrl;
    QString htmlUrl;
    bool isPrivate;
    int stars;
    int forks;
};

struct GitHubUser {
    QString login;
    QString name;
    QString email;
    QString avatarUrl;
};

struct GitHubPullRequest {
    int number;
    QString title;
    QString body;
    QString state;
    QString author;
    QString createdAt;
};

class GitHubClient : public QObject
{
    Q_OBJECT

public:
    explicit GitHubClient(QObject *parent = nullptr);
    ~GitHubClient();
    
    void setToken(const QString &token);
    
    // User operations
    void getUser();
    void getUserRepositories();
    
    // Repository operations
    void getRepository(const QString &owner, const QString &repo);
    void createRepository(const QString &name, const QString &description, bool isPrivate);
    void forkRepository(const QString &owner, const QString &repo);
    
    // Pull request operations
    void getPullRequests(const QString &owner, const QString &repo);
    void createPullRequest(const QString &owner, const QString &repo,
                          const QString &title, const QString &body,
                          const QString &head, const QString &base);
    
    // Issue operations
    void getIssues(const QString &owner, const QString &repo);
    void createIssue(const QString &owner, const QString &repo,
                    const QString &title, const QString &body);

signals:
    void userReceived(const GitHubUser &user);
    void repositoriesReceived(const QList<GitHubRepository> &repos);
    void repositoryReceived(const GitHubRepository &repo);
    void pullRequestsReceived(const QList<GitHubPullRequest> &prs);
    void error(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkRequest createRequest(const QString &endpoint);
    void sendRequest(const QString &endpoint, const QString &method = "GET",
                    const QJsonDocument &data = QJsonDocument());
    
    QNetworkAccessManager *m_networkManager;
    QString m_token;
    QString m_apiBase;
};

#endif // GITHUBCLIENT_H
