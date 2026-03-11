#include "mainwindow.h"
#include "git/gitrepository.h"
#include "widgets/repositoryview.h"
#include "widgets/changesview.h"
#include "widgets/historyview.h"
#include "widgets/branchview.h"
#include "dialogs/clonedialog.h"
#include "dialogs/commitdialog.h"
#include "dialogs/branchdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/accountdialog.h"
#include "utils/accountmanager.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_toolbar(nullptr)
    , m_statusBar(nullptr)
    , m_mainSplitter(nullptr)
    , m_contentStack(nullptr)
    , m_repoView(nullptr)
    , m_changesView(nullptr)
    , m_historyView(nullptr)
    , m_branchView(nullptr)
{
    setWindowTitle("Aurora - Git Client");
    resize(1200, 800);
    
    setupUI();
    createActions();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    applyModernTheme();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Create left sidebar with repository list
    m_repoView = new RepositoryView(this);
    connect(m_repoView, &RepositoryView::repositorySelected, 
            this, &MainWindow::onRepositoryChanged);
    
    // Create content stack
    m_contentStack = new QStackedWidget(this);
    
    // Create views
    m_changesView = new ChangesView(this);
    connect(m_changesView, &ChangesView::commitRequested, this, &MainWindow::onCommit);
    
    m_historyView = new HistoryView(this);
    m_branchView = new BranchView(this);
    
    m_contentStack->addWidget(m_changesView);
    m_contentStack->addWidget(m_historyView);
    m_contentStack->addWidget(m_branchView);
    
    // Add to splitter
    m_mainSplitter->addWidget(m_repoView);
    m_mainSplitter->addWidget(m_contentStack);
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 3);
    
    mainLayout->addWidget(m_mainSplitter);
}

void MainWindow::createActions()
{
    m_cloneAction = new QAction(tr("Clone Repository"), this);
    m_cloneAction->setShortcut(QKeySequence::New);
    connect(m_cloneAction, &QAction::triggered, this, &MainWindow::onCloneRepository);
    
    m_openAction = new QAction(tr("Open Repository"), this);
    m_openAction->setShortcut(QKeySequence::Open);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenRepository);
    
    m_createAction = new QAction(tr("Create Repository"), this);
    connect(m_createAction, &QAction::triggered, this, &MainWindow::onCreateRepository);
    
    m_commitAction = new QAction(tr("Commit"), this);
    m_commitAction->setShortcut(QKeySequence(tr("Ctrl+Return")));
    connect(m_commitAction, &QAction::triggered, this, &MainWindow::onCommit);
    
    m_pushAction = new QAction(tr("Push"), this);
    m_pushAction->setShortcut(QKeySequence(tr("Ctrl+P")));
    connect(m_pushAction, &QAction::triggered, this, &MainWindow::onPush);
    
    m_pullAction = new QAction(tr("Pull"), this);
    m_pullAction->setShortcut(QKeySequence(tr("Ctrl+Shift+P")));
    connect(m_pullAction, &QAction::triggered, this, &MainWindow::onPull);
    
    m_fetchAction = new QAction(tr("Fetch"), this);
    m_fetchAction->setShortcut(QKeySequence(tr("Ctrl+Shift+F")));
    connect(m_fetchAction, &QAction::triggered, this, &MainWindow::onFetch);
    
    m_refreshAction = new QAction(tr("Refresh"), this);
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    connect(m_refreshAction, &QAction::triggered, this, &MainWindow::onRefresh);
    
    m_branchAction = new QAction(tr("New Branch"), this);
    m_branchAction->setShortcut(QKeySequence(tr("Ctrl+B")));
    connect(m_branchAction, &QAction::triggered, this, &MainWindow::onCreateBranch);
    
    m_settingsAction = new QAction(tr("Settings"), this);
    m_settingsAction->setShortcut(QKeySequence::Preferences);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettings);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(m_cloneAction);
    fileMenu->addAction(m_openAction);
    fileMenu->addAction(m_createAction);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), this, &QWidget::close, QKeySequence::Quit);
    
    // Repository menu
    QMenu *repoMenu = menuBar->addMenu(tr("&Repository"));
    repoMenu->addAction(m_commitAction);
    repoMenu->addAction(m_pushAction);
    repoMenu->addAction(m_pullAction);
    repoMenu->addAction(m_fetchAction);
    repoMenu->addSeparator();
    repoMenu->addAction(m_refreshAction);
    
    // Branch menu
    QMenu *branchMenu = menuBar->addMenu(tr("&Branch"));
    branchMenu->addAction(m_branchAction);
    branchMenu->addAction(tr("Switch Branch"), this, &MainWindow::onSwitchBranch);
    
    // View menu
    QMenu *viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addAction(tr("Changes"), [this]() { m_contentStack->setCurrentWidget(m_changesView); });
    viewMenu->addAction(tr("History"), [this]() { m_contentStack->setCurrentWidget(m_historyView); });
    viewMenu->addAction(tr("Branches"), [this]() { m_contentStack->setCurrentWidget(m_branchView); });
    
    // Account menu
    QMenu *accountMenu = menuBar->addMenu(tr("&Account"));
    accountMenu->addAction(tr("Manage Accounts"), [this]() {
        AccountDialog dialog(this);
        dialog.exec();
        updateWindowTitle(); // Refresh in case account changed
    });
    accountMenu->addSeparator();
    
    // Quick account switcher
    AccountManager accountMgr;
    auto accounts = accountMgr.accounts();
    GitAccount current = accountMgr.currentAccount();
    
    for (const auto &account : accounts) {
        QAction *accountAction = accountMenu->addAction(account.name);
        accountAction->setCheckable(true);
        accountAction->setChecked(account.name == current.name);
        connect(accountAction, &QAction::triggered, [this, account]() {
            AccountManager mgr;
            mgr.setCurrentAccount(account.name);
            QMessageBox::information(this, tr("Account Switched"),
                tr("Switched to: %1 <%2>").arg(account.userName, account.userEmail));
            updateWindowTitle();
        });
    }
    
    // Help menu
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(tr("About"), [this]() {
        QMessageBox::about(this, tr("About Aurora"),
            tr("Aurora Git Client\nVersion 0.1.0\n\nA modern, simple Git client built with Qt."));
    });
}

void MainWindow::setupToolBar()
{
    m_toolbar = addToolBar(tr("Main Toolbar"));
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(QSize(24, 24));
    
    m_toolbar->addAction(m_cloneAction);
    m_toolbar->addAction(m_openAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_commitAction);
    m_toolbar->addAction(m_pushAction);
    m_toolbar->addAction(m_pullAction);
    m_toolbar->addAction(m_fetchAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_branchAction);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_refreshAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage(tr("Ready"));
}

void MainWindow::applyModernTheme()
{
    // Apply a modern dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    QApplication::setPalette(darkPalette);
    
    // Apply stylesheet for additional styling
    QString styleSheet = R"(
        QMainWindow {
            background-color: #2d2d30;
        }
        QToolBar {
            background-color: #3e3e42;
            border: none;
            padding: 4px;
            spacing: 4px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 4px;
            border-radius: 3px;
        }
        QToolButton:hover {
            background-color: #505052;
        }
        QToolButton:pressed {
            background-color: #007acc;
        }
        QMenuBar {
            background-color: #3e3e42;
            color: white;
        }
        QMenuBar::item:selected {
            background-color: #505052;
        }
        QMenu {
            background-color: #3e3e42;
            color: white;
            border: 1px solid #555;
        }
        QMenu::item:selected {
            background-color: #007acc;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
        }
        QPushButton {
            background-color: #0e639c;
            color: white;
            border: none;
            padding: 6px 12px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QPushButton:pressed {
            background-color: #007acc;
        }
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: #1e1e1e;
            color: white;
            border: 1px solid #555;
            padding: 4px;
            border-radius: 3px;
        }
        QListWidget, QTreeWidget, QTableWidget {
            background-color: #1e1e1e;
            color: white;
            border: 1px solid #555;
            alternate-background-color: #252526;
        }
        QListWidget::item:selected, QTreeWidget::item:selected, QTableWidget::item:selected {
            background-color: #007acc;
        }
        QSplitter::handle {
            background-color: #555;
        }
    )";
    
    setStyleSheet(styleSheet);
}

void MainWindow::onCloneRepository()
{
    CloneDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString url = dialog.getUrl();
        QString path = dialog.getPath();
        
        m_statusBar->showMessage(tr("Cloning repository..."));
        
        // TODO: Implement clone in background thread
        try {
            GitRepository::clone(url, path);
            loadRepository(path);
            m_statusBar->showMessage(tr("Repository cloned successfully"), 3000);
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("Clone Error"), 
                tr("Failed to clone repository: %1").arg(e.what()));
            m_statusBar->showMessage(tr("Clone failed"), 3000);
        }
    }
}

void MainWindow::onOpenRepository()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Repository"));
    if (!path.isEmpty()) {
        loadRepository(path);
    }
}

void MainWindow::onCreateRepository()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Create Repository"));
    if (!path.isEmpty()) {
        try {
            GitRepository::init(path);
            loadRepository(path);
            m_statusBar->showMessage(tr("Repository created successfully"), 3000);
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("Create Error"), 
                tr("Failed to create repository: %1").arg(e.what()));
        }
    }
}

void MainWindow::onSettings()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onRefresh()
{
    if (m_repository) {
        m_changesView->refresh();
        m_historyView->refresh();
        m_branchView->refresh();
        m_statusBar->showMessage(tr("Refreshed"), 2000);
    }
}

void MainWindow::onCommit()
{
    if (!m_repository) {
        QMessageBox::warning(this, tr("No Repository"), 
            tr("Please open a repository first."));
        return;
    }
    
    CommitDialog dialog(m_repository, this);
    if (dialog.exec() == QDialog::Accepted) {
        onRefresh();
        m_statusBar->showMessage(tr("Changes committed"), 3000);
    }
}

void MainWindow::onPush()
{
    if (!m_repository) return;
    
    m_statusBar->showMessage(tr("Pushing..."));
    try {
        m_repository->push();
        m_statusBar->showMessage(tr("Pushed successfully"), 3000);
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Push Error"), 
            tr("Failed to push: %1").arg(e.what()));
        m_statusBar->showMessage(tr("Push failed"), 3000);
    }
}

void MainWindow::onPull()
{
    if (!m_repository) return;
    
    m_statusBar->showMessage(tr("Pulling..."));
    try {
        m_repository->pull();
        onRefresh();
        m_statusBar->showMessage(tr("Pulled successfully"), 3000);
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Pull Error"), 
            tr("Failed to pull: %1").arg(e.what()));
        m_statusBar->showMessage(tr("Pull failed"), 3000);
    }
}

void MainWindow::onFetch()
{
    if (!m_repository) return;
    
    m_statusBar->showMessage(tr("Fetching..."));
    try {
        m_repository->fetch();
        onRefresh();
        m_statusBar->showMessage(tr("Fetched successfully"), 3000);
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Fetch Error"), 
            tr("Failed to fetch: %1").arg(e.what()));
        m_statusBar->showMessage(tr("Fetch failed"), 3000);
    }
}

void MainWindow::onCreateBranch()
{
    if (!m_repository) {
        QMessageBox::warning(this, tr("No Repository"), 
            tr("Please open a repository first."));
        return;
    }
    
    BranchDialog dialog(m_repository, this);
    if (dialog.exec() == QDialog::Accepted) {
        onRefresh();
    }
}

void MainWindow::onSwitchBranch()
{
    if (!m_repository) return;
    
    // Get list of branches
    auto branches = m_repository->getBranches();
    QString currentBranch = m_repository->currentBranch();
    
    QStringList branchNames;
    for (const auto &branch : branches) {
        if (!branch.isRemote) {
            branchNames << branch.name;
        }
    }
    
    if (branchNames.isEmpty()) {
        QMessageBox::information(this, tr("No Branches"),
            tr("No local branches available."));
        return;
    }
    
    bool ok;
    QString selected = QInputDialog::getItem(this, tr("Switch Branch"),
                                            tr("Select branch to checkout:"),
                                            branchNames, 0, false, &ok);
    
    if (ok && !selected.isEmpty() && selected != currentBranch) {
        try {
            m_repository->checkoutBranch(selected);
            onRefresh();
            m_statusBar->showMessage(tr("Switched to branch: %1").arg(selected), 3000);
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("Checkout Error"),
                tr("Failed to switch branch: %1").arg(e.what()));
        }
    }
}

void MainWindow::onRepositoryChanged(const QString &path)
{
    loadRepository(path);
}

void MainWindow::loadRepository(const QString &path)
{
    try {
        m_repository = std::make_shared<GitRepository>(path);
        m_currentRepoPath = path;
        
        // Configure repository with current account
        AccountManager accountMgr;
        GitAccount account = accountMgr.currentAccount();
        if (account.isValid()) {
            m_repository->configureUser(account.userName, account.userEmail);
        }
        
        // Update views
        m_changesView->setRepository(m_repository);
        m_historyView->setRepository(m_repository);
        m_branchView->setRepository(m_repository);
        
        // Add to recent repositories
        m_repoView->addRepository(path);
        
        updateWindowTitle();
        m_statusBar->showMessage(tr("Repository loaded: %1").arg(path), 3000);
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Load Error"), 
            tr("Failed to load repository: %1").arg(e.what()));
    }
}

void MainWindow::updateWindowTitle()
{
    if (m_repository) {
        QString repoName = QFileInfo(m_currentRepoPath).fileName();
        setWindowTitle(tr("Aurora - %1").arg(repoName));
    } else {
        setWindowTitle(tr("Aurora - Git Client"));
    }
}
