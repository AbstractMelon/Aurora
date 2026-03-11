#ifndef BRANCHVIEW_H
#define BRANCHVIEW_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <memory>

class GitRepository;

class BranchView : public QWidget
{
    Q_OBJECT

public:
    explicit BranchView(QWidget *parent = nullptr);
    
    void setRepository(std::shared_ptr<GitRepository> repo);
    void refresh();

private slots:
    void onCreateBranch();
    void onDeleteBranch();
    void onCheckoutBranch();
    void onContextMenu(const QPoint &pos);

private:
    void updateBranchList();
    
    std::shared_ptr<GitRepository> m_repository;
    QTreeWidget *m_branchTree;
    QPushButton *m_createButton;
    QPushButton *m_deleteButton;
    QPushButton *m_checkoutButton;
};

#endif // BRANCHVIEW_H
