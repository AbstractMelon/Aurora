#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QListWidget>
#include <memory>

class GitRepository;
class RepositoryView;
class ChangesView;
class HistoryView;
class BranchView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCloneRepository();
    void onOpenRepository();
    void onCreateRepository();
    void onSettings();
    void onRefresh();
    void onCommit();
    void onPush();
    void onPull();
    void onFetch();
    void onCreateBranch();
    void onSwitchBranch();
    void onRepositoryChanged(const QString &path);

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void createActions();
    void loadRepository(const QString &path);
    void updateWindowTitle();
    void applyModernTheme();

    // UI Components
    QToolBar *m_toolbar;
    QStatusBar *m_statusBar;
    QSplitter *m_mainSplitter;
    QStackedWidget *m_contentStack;
    
    // Views
    RepositoryView *m_repoView;
    ChangesView *m_changesView;
    HistoryView *m_historyView;
    BranchView *m_branchView;
    
    // Actions
    QAction *m_cloneAction;
    QAction *m_openAction;
    QAction *m_createAction;
    QAction *m_commitAction;
    QAction *m_pushAction;
    QAction *m_pullAction;
    QAction *m_fetchAction;
    QAction *m_refreshAction;
    QAction *m_branchAction;
    QAction *m_settingsAction;
    
    // Current repository
    std::shared_ptr<GitRepository> m_repository;
    QString m_currentRepoPath;
};

#endif // MAINWINDOW_H
