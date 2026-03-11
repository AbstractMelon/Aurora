#include "githubclient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GitHubClient::GitHubClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_apiBase("https://api.github.com")
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &GitHubClient::onReplyFinished);
}

GitHubClient::~GitHubClient()
{
}

void GitHubClient::setToken(const QString &token)
{
    m_token = token;
}

QNetworkRequest GitHubClient::createRequest(const QString &endpoint)
{
    QUrl url(m_apiBase + endpoint);
    QNetworkRequest request(url);
    
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");
    
    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("token %1").arg(m_token).toUtf8());
    }
    
    return request;
}

void GitHubClient::sendRequest(const QString &endpoint, const QString &method, 
                               const QJsonDocument &data)
{
    QNetworkRequest request = createRequest(endpoint);
    
    if (method == "GET") {
        m_networkManager->get(request);
    } else if (method == "POST") {
        m_networkManager->post(request, data.toJson());
    } else if (method == "PUT") {
        m_networkManager->put(request, data.toJson());
    } else if (method == "DELETE") {
        m_networkManager->deleteResource(request);
    }
}

void GitHubClient::getUser()
{
    sendRequest("/user");
}

void GitHubClient::getUserRepositories()
{
    sendRequest("/user/repos");
}

void GitHubClient::getRepository(const QString &owner, const QString &repo)
{
    sendRequest(QString("/repos/%1/%2").arg(owner, repo));
}

void GitHubClient::createRepository(const QString &name, const QString &description, 
                                   bool isPrivate)
{
    QJsonObject data;
    data["name"] = name;
    data["description"] = description;
    data["private"] = isPrivate;
    
    sendRequest("/user/repos", "POST", QJsonDocument(data));
}

void GitHubClient::forkRepository(const QString &owner, const QString &repo)
{
    sendRequest(QString("/repos/%1/%2/forks").arg(owner, repo), "POST");
}

void GitHubClient::getPullRequests(const QString &owner, const QString &repo)
{
    sendRequest(QString("/repos/%1/%2/pulls").arg(owner, repo));
}

void GitHubClient::createPullRequest(const QString &owner, const QString &repo,
                                    const QString &title, const QString &body,
                                    const QString &head, const QString &base)
{
    QJsonObject data;
    data["title"] = title;
    data["body"] = body;
    data["head"] = head;
    data["base"] = base;
    
    sendRequest(QString("/repos/%1/%2/pulls").arg(owner, repo), "POST", 
               QJsonDocument(data));
}

void GitHubClient::getIssues(const QString &owner, const QString &repo)
{
    sendRequest(QString("/repos/%1/%2/issues").arg(owner, repo));
}

void GitHubClient::createIssue(const QString &owner, const QString &repo,
                              const QString &title, const QString &body)
{
    QJsonObject data;
    data["title"] = title;
    data["body"] = body;
    
    sendRequest(QString("/repos/%1/%2/issues").arg(owner, repo), "POST", 
               QJsonDocument(data));
}

void GitHubClient::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    QString endpoint = reply->url().path();
    
    if (endpoint == "/user") {
        GitHubUser user;
        QJsonObject obj = doc.object();
        user.login = obj["login"].toString();
        user.name = obj["name"].toString();
        user.email = obj["email"].toString();
        user.avatarUrl = obj["avatar_url"].toString();
        emit userReceived(user);
    }
    else if (endpoint.endsWith("/repos") || endpoint == "/user/repos") {
        QList<GitHubRepository> repos;
        QJsonArray arr = doc.array();
        
        for (const QJsonValue &val : arr) {
            QJsonObject obj = val.toObject();
            GitHubRepository repo;
            repo.name = obj["name"].toString();
            repo.fullName = obj["full_name"].toString();
            repo.description = obj["description"].toString();
            repo.cloneUrl = obj["clone_url"].toString();
            repo.htmlUrl = obj["html_url"].toString();
            repo.isPrivate = obj["private"].toBool();
            repo.stars = obj["stargazers_count"].toInt();
            repo.forks = obj["forks_count"].toInt();
            repos.append(repo);
        }
        
        emit repositoriesReceived(repos);
    }
    else if (endpoint.contains("/repos/") && !endpoint.contains("/pulls") && 
             !endpoint.contains("/issues")) {
        GitHubRepository repo;
        QJsonObject obj = doc.object();
        repo.name = obj["name"].toString();
        repo.fullName = obj["full_name"].toString();
        repo.description = obj["description"].toString();
        repo.cloneUrl = obj["clone_url"].toString();
        repo.htmlUrl = obj["html_url"].toString();
        repo.isPrivate = obj["private"].toBool();
        repo.stars = obj["stargazers_count"].toInt();
        repo.forks = obj["forks_count"].toInt();
        emit repositoryReceived(repo);
    }
    else if (endpoint.contains("/pulls")) {
        QList<GitHubPullRequest> prs;
        QJsonArray arr = doc.array();
        
        for (const QJsonValue &val : arr) {
            QJsonObject obj = val.toObject();
            GitHubPullRequest pr;
            pr.number = obj["number"].toInt();
            pr.title = obj["title"].toString();
            pr.body = obj["body"].toString();
            pr.state = obj["state"].toString();
            pr.author = obj["user"].toObject()["login"].toString();
            pr.createdAt = obj["created_at"].toString();
            prs.append(pr);
        }
        
        emit pullRequestsReceived(prs);
    }
    
    reply->deleteLater();
}
