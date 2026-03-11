#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <memory>

class GitRepository;

class HistoryView : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryView(QWidget *parent = nullptr);
    
    void setRepository(std::shared_ptr<GitRepository> repo);
    void refresh();

private slots:
    void onCommitSelected();

private:
    void updateCommitList();
    
    std::shared_ptr<GitRepository> m_repository;
    QTableWidget *m_commitTable;
    QTextEdit *m_commitDetails;
};

#endif // HISTORYVIEW_H
