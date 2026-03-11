#include "historyview.h"
#include "git/gitrepository.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QHeaderView>

HistoryView::HistoryView(QWidget *parent)
    : QWidget(parent)
    , m_repository(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    QLabel *title = new QLabel(tr("History"), this);
    QFont font = title->font();
    font.setPointSize(14);
    font.setBold(true);
    title->setFont(font);
    mainLayout->addWidget(title);
    
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    
    // Commit table
    m_commitTable = new QTableWidget(this);
    m_commitTable->setColumnCount(4);
    m_commitTable->setHorizontalHeaderLabels({tr("Hash"), tr("Message"), tr("Author"), tr("Date")});
    m_commitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_commitTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_commitTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_commitTable->horizontalHeader()->setStretchLastSection(true);
    m_commitTable->setColumnWidth(0, 100);
    m_commitTable->setColumnWidth(1, 300);
    m_commitTable->setColumnWidth(2, 150);
    
    connect(m_commitTable, &QTableWidget::itemSelectionChanged,
            this, &HistoryView::onCommitSelected);
    
    // Commit details
    m_commitDetails = new QTextEdit(this);
    m_commitDetails->setReadOnly(true);
    
    splitter->addWidget(m_commitTable);
    splitter->addWidget(m_commitDetails);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
}

void HistoryView::setRepository(std::shared_ptr<GitRepository> repo)
{
    m_repository = repo;
    refresh();
}

void HistoryView::refresh()
{
    if (!m_repository) return;
    
    updateCommitList();
}

void HistoryView::updateCommitList()
{
    m_commitTable->setRowCount(0);
    
    if (!m_repository) return;
    
    auto commits = m_repository->log(100);
    
    m_commitTable->setRowCount(commits.size());
    
    for (size_t i = 0; i < commits.size(); i++) {
        const auto &commit = commits[i];
        
        // Hash (shortened)
        QString shortHash = commit.oid.left(8);
        QTableWidgetItem *hashItem = new QTableWidgetItem(shortHash);
        hashItem->setData(Qt::UserRole, commit.oid);
        m_commitTable->setItem(i, 0, hashItem);
        
        // Message (first line only)
        QString message = commit.message.split('\n').first();
        m_commitTable->setItem(i, 1, new QTableWidgetItem(message));
        
        // Author
        m_commitTable->setItem(i, 2, new QTableWidgetItem(commit.author));
        
        // Date
        QString date = commit.timestamp.toString("yyyy-MM-dd HH:mm");
        m_commitTable->setItem(i, 3, new QTableWidgetItem(date));
    }
}

void HistoryView::onCommitSelected()
{
    auto selected = m_commitTable->selectedItems();
    if (selected.isEmpty() || !m_repository) {
        m_commitDetails->clear();
        return;
    }
    
    int row = selected.first()->row();
    QString oid = m_commitTable->item(row, 0)->data(Qt::UserRole).toString();
    
    auto commits = m_repository->log(100);
    
    for (const auto &commit : commits) {
        if (commit.oid == oid) {
            QString details = QString("Commit: %1\n").arg(commit.oid);
            details += QString("Author: %1 <%2>\n").arg(commit.author, commit.email);
            details += QString("Date: %1\n\n").arg(commit.timestamp.toString());
            details += commit.message;
            
            m_commitDetails->setPlainText(details);
            break;
        }
    }
}
