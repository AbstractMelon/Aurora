#ifndef CHANGESVIEW_H
#define CHANGESVIEW_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QMap>
#include <QSet>
#include <memory>

class GitRepository;

class ChangesView : public QWidget
{
    Q_OBJECT

public:
    explicit ChangesView(QWidget *parent = nullptr);
    
    void setRepository(std::shared_ptr<GitRepository> repo);
    void refresh();

signals:
    void commitRequested();

private slots:
    void onItemDoubleClicked(QListWidgetItem *item);
    void onStageAll();
    void onUnstageAll();
    void onCommit();
    void onSelectionChanged();
    void onStageSelected();
    void onUnstageSelected();

private:
    void updateFileList();
    QString statusToString(int status);
    QString statusToIcon(int status);
    QColor statusToColor(int status);
    
    std::shared_ptr<GitRepository> m_repository;
    QListWidget *m_unstagedList;
    QListWidget *m_stagedList;
    QTextEdit *m_diffView;
    QPushButton *m_stageButton;
    QPushButton *m_unstageButton;
    QPushButton *m_stageAllButton;
    QPushButton *m_unstageAllButton;
    QPushButton *m_commitButton;
    
    QSet<QString> m_stagedFiles; // Track which files are staged
};

#endif // CHANGESVIEW_H
