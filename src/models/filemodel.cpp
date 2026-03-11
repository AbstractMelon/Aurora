#include "filemodel.h"

FileModel::FileModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void FileModel::setFiles(const std::vector<GitFileStatus> &files)
{
    beginResetModel();
    m_files = files;
    endResetModel();
}

void FileModel::clear()
{
    beginResetModel();
    m_files.clear();
    endResetModel();
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_files.size();
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_files.size()))
        return QVariant();
    
    const GitFileStatus &file = m_files[index.row()];
    
    if (role == Qt::DisplayRole) {
        return file.path;
    }
    else if (role == Qt::UserRole) {
        return static_cast<int>(file.status);
    }
    
    return QVariant();
}
