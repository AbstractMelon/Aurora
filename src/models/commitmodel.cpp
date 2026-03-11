#include "commitmodel.h"

CommitModel::CommitModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void CommitModel::setCommits(const std::vector<GitCommit> &commits)
{
    beginResetModel();
    m_commits = commits;
    endResetModel();
}

void CommitModel::clear()
{
    beginResetModel();
    m_commits.clear();
    endResetModel();
}

int CommitModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_commits.size();
}

int CommitModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 4; // Hash, Message, Author, Date
}

QVariant CommitModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_commits.size()))
        return QVariant();
    
    const GitCommit &commit = m_commits[index.row()];
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return commit.oid.left(8);
            case 1: return commit.message.split('\n').first();
            case 2: return commit.author;
            case 3: return commit.timestamp.toString("yyyy-MM-dd HH:mm");
        }
    }
    else if (role == Qt::UserRole) {
        return commit.oid;
    }
    
    return QVariant();
}

QVariant CommitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Hash");
            case 1: return tr("Message");
            case 2: return tr("Author");
            case 3: return tr("Date");
        }
    }
    
    return QVariant();
}
