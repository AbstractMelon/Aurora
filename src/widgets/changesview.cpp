#include "changesview.h"
#include "git/gitrepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QGroupBox>
#include <QFileInfo>
#include <QDebug>

ChangesView::ChangesView(QWidget *parent)
    : QWidget(parent)
    , m_repository(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    QLabel *title = new QLabel(tr("Changes"), this);
    QFont font = title->font();
    font.setPointSize(14);
    font.setBold(true);
    title->setFont(font);
    mainLayout->addWidget(title);
    
    // Main splitter: file lists on left, diff on right
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Left side: file lists
    QWidget *listsWidget = new QWidget(this);
    QVBoxLayout *listsLayout = new QVBoxLayout(listsWidget);
    listsLayout->setContentsMargins(0, 0, 0, 0);
    
    // Unstaged changes section
    QGroupBox *unstagedGroup = new QGroupBox(tr("Unstaged Changes"), this);
    QVBoxLayout *unstagedLayout = new QVBoxLayout(unstagedGroup);
    
    m_unstagedList = new QListWidget(this);
    m_unstagedList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(m_unstagedList, &QListWidget::itemDoubleClicked, 
            this, &ChangesView::onStageSelected);
    connect(m_unstagedList, &QListWidget::itemSelectionChanged,
            this, &ChangesView::onSelectionChanged);
    unstagedLayout->addWidget(m_unstagedList);
    
    QHBoxLayout *unstagedButtons = new QHBoxLayout();
    m_stageButton = new QPushButton(tr("Stage →"), this);
    m_stageAllButton = new QPushButton(tr("Stage All →"), this);
    connect(m_stageButton, &QPushButton::clicked, this, &ChangesView::onStageSelected);
    connect(m_stageAllButton, &QPushButton::clicked, this, &ChangesView::onStageAll);
    unstagedButtons->addWidget(m_stageButton);
    unstagedButtons->addWidget(m_stageAllButton);
    unstagedLayout->addLayout(unstagedButtons);
    
    listsLayout->addWidget(unstagedGroup);
    
    // Staged changes section
    QGroupBox *stagedGroup = new QGroupBox(tr("Staged Changes"), this);
    QVBoxLayout *stagedLayout = new QVBoxLayout(stagedGroup);
    
    m_stagedList = new QListWidget(this);
    m_stagedList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(m_stagedList, &QListWidget::itemDoubleClicked, 
            this, &ChangesView::onUnstageSelected);
    connect(m_stagedList, &QListWidget::itemSelectionChanged,
            this, &ChangesView::onSelectionChanged);
    stagedLayout->addWidget(m_stagedList);
    
    QHBoxLayout *stagedButtons = new QHBoxLayout();
    m_unstageButton = new QPushButton(tr("← Unstage"), this);
    m_unstageAllButton = new QPushButton(tr("← Unstage All"), this);
    connect(m_unstageButton, &QPushButton::clicked, this, &ChangesView::onUnstageSelected);
    connect(m_unstageAllButton, &QPushButton::clicked, this, &ChangesView::onUnstageAll);
    stagedButtons->addWidget(m_unstageButton);
    stagedButtons->addWidget(m_unstageAllButton);
    stagedLayout->addLayout(stagedButtons);
    
    listsLayout->addWidget(stagedGroup);
    
    // Commit button at bottom
    m_commitButton = new QPushButton(tr("Commit Staged Changes"), this);
    m_commitButton->setMinimumHeight(40);
    QFont commitFont = m_commitButton->font();
    commitFont.setPointSize(11);
    commitFont.setBold(true);
    m_commitButton->setFont(commitFont);
    connect(m_commitButton, &QPushButton::clicked, this, &ChangesView::onCommit);
    listsLayout->addWidget(m_commitButton);
    
    mainSplitter->addWidget(listsWidget);
    
    // Right side: diff view
    QWidget *diffWidget = new QWidget(this);
    QVBoxLayout *diffLayout = new QVBoxLayout(diffWidget);
    diffLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *diffLabel = new QLabel(tr("Diff Preview"), this);
    QFont diffLabelFont = diffLabel->font();
    diffLabelFont.setBold(true);
    diffLabel->setFont(diffLabelFont);
    diffLayout->addWidget(diffLabel);
    
    m_diffView = new QTextEdit(this);
    m_diffView->setReadOnly(true);
    m_diffView->setFont(QFont("Monospace", 9));
    m_diffView->setLineWrapMode(QTextEdit::NoWrap);
    diffLayout->addWidget(m_diffView);
    
    mainSplitter->addWidget(diffWidget);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(mainSplitter);
}

void ChangesView::setRepository(std::shared_ptr<GitRepository> repo)
{
    m_repository = repo;
    m_stagedFiles.clear();
    refresh();
}

void ChangesView::refresh()
{
    if (!m_repository) return;
    
    updateFileList();
}

void ChangesView::updateFileList()
{
    m_unstagedList->clear();
    m_stagedList->clear();
    
    if (!m_repository) return;
    
    try {
        auto files = m_repository->status();
        
        for (const auto &file : files) {
            QString displayText = QString("%1  %2").arg(statusToIcon(file.status), file.path);
            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, file.path);
            item->setData(Qt::UserRole + 1, file.status);
            item->setForeground(statusToColor(file.status));
            
            if (m_stagedFiles.contains(file.path)) {
                m_stagedList->addItem(item);
            } else {
                m_unstagedList->addItem(item);
            }
        }
        
        qDebug() << "Updated file list:" << files.size() << "files," << m_stagedFiles.size() << "staged";
    } catch (const std::exception &e) {
        qDebug() << "Error getting status:" << e.what();
    }
}

QString ChangesView::statusToString(int status)
{
    switch (status) {
        case 0: return tr("Unmodified");
        case 1: return tr("Added");
        case 2: return tr("Modified");
        case 3: return tr("Deleted");
        case 4: return tr("Renamed");
        case 5: return tr("Untracked");
        case 6: return tr("Conflicted");
        default: return tr("Unknown");
    }
}

QString ChangesView::statusToIcon(int status)
{
    switch (status) {
        case 1: return "✚";  // Added
        case 2: return "●";  // Modified
        case 3: return "✖";  // Deleted
        case 4: return "➜";  // Renamed
        case 5: return "?";  // Untracked
        case 6: return "⚠";  // Conflicted
        default: return "•";
    }
}

QColor ChangesView::statusToColor(int status)
{
    switch (status) {
        case 1: return QColor(0, 200, 0);      // Added - green
        case 2: return QColor(255, 165, 0);    // Modified - orange
        case 3: return QColor(255, 0, 0);      // Deleted - red
        case 4: return QColor(100, 149, 237);  // Renamed - blue
        case 5: return QColor(180, 180, 180);  // Untracked - gray
        case 6: return QColor(255, 0, 255);    // Conflicted - magenta
        default: return QColor(255, 255, 255); // Default - white
    }
}

void ChangesView::onStageSelected()
{
    if (!m_repository) return;
    
    auto selected = m_unstagedList->selectedItems();
    for (auto *item : selected) {
        QString path = item->data(Qt::UserRole).toString();
        try {
            m_repository->stageFile(path);
            m_stagedFiles.insert(path);
            qDebug() << "Staged:" << path;
        } catch (const std::exception &e) {
            qDebug() << "Error staging:" << e.what();
        }
    }
    
    refresh();
}

void ChangesView::onUnstageSelected()
{
    if (!m_repository) return;
    
    auto selected = m_stagedList->selectedItems();
    for (auto *item : selected) {
        QString path = item->data(Qt::UserRole).toString();
        try {
            m_repository->unstageFile(path);
            m_stagedFiles.remove(path);
            qDebug() << "Unstaged:" << path;
        } catch (const std::exception &e) {
            qDebug() << "Error unstaging:" << e.what();
        }
    }
    
    refresh();
}

void ChangesView::onStageAll()
{
    if (!m_repository) return;
    
    try {
        m_repository->stageAll();
        
        // Mark all unstaged files as staged
        for (int i = 0; i < m_unstagedList->count(); ++i) {
            QString path = m_unstagedList->item(i)->data(Qt::UserRole).toString();
            m_stagedFiles.insert(path);
        }
        
        refresh();
        qDebug() << "Staged all files";
    } catch (const std::exception &e) {
        qDebug() << "Error staging all:" << e.what();
    }
}

void ChangesView::onUnstageAll()
{
    if (!m_repository) return;
    
    try {
        // Unstage each file individually
        QSet<QString> toUnstage = m_stagedFiles;
        for (const QString &path : toUnstage) {
            m_repository->unstageFile(path);
            m_stagedFiles.remove(path);
        }
        
        refresh();
        qDebug() << "Unstaged all files";
    } catch (const std::exception &e) {
        qDebug() << "Error unstaging all:" << e.what();
    }
}

void ChangesView::onCommit()
{
    emit commitRequested();
}

void ChangesView::onItemDoubleClicked(QListWidgetItem *item)
{
    // Double-click to stage/unstage
    Q_UNUSED(item);
}

void ChangesView::onSelectionChanged()
{
    QListWidgetItem *item = nullptr;
    
    // Check which list has selection
    if (!m_unstagedList->selectedItems().isEmpty()) {
        item = m_unstagedList->selectedItems().first();
    } else if (!m_stagedList->selectedItems().isEmpty()) {
        item = m_stagedList->selectedItems().first();
    }
    
    if (!item || !m_repository) {
        m_diffView->clear();
        return;
    }
    
    QString path = item->data(Qt::UserRole).toString();
    try {
        QString diff = m_repository->diff(path);
        m_diffView->setPlainText(diff);
    } catch (const std::exception &e) {
        m_diffView->setPlainText(QString("Error getting diff: %1").arg(e.what()));
    }
}
