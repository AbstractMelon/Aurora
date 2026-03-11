#ifndef REPOSITORYVIEW_H
#define REPOSITORYVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>

class RepositoryView : public QWidget
{
    Q_OBJECT

public:
    explicit RepositoryView(QWidget *parent = nullptr);
    
    void addRepository(const QString &path);
    void removeRepository(const QString &path);
    QStringList repositories() const;

signals:
    void repositorySelected(const QString &path);

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onAddClicked();

private:
    void loadRepositories();
    void saveRepositories();
    
    QListWidget *m_listWidget;
    QPushButton *m_addButton;
};

#endif // REPOSITORYVIEW_H
