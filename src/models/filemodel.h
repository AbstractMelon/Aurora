#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "git/gitrepository.h"

class FileModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileModel(QObject *parent = nullptr);
    
    void setFiles(const std::vector<GitFileStatus> &files);
    void clear();
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<GitFileStatus> m_files;
};

#endif // FILEMODEL_H
