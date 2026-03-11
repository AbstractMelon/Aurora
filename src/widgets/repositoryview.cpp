#include "repositoryview.h"
#include "utils/settings.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>

RepositoryView::RepositoryView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    
    QLabel *title = new QLabel(tr("Repositories"), this);
    QFont font = title->font();
    font.setPointSize(12);
    font.setBold(true);
    title->setFont(font);
    layout->addWidget(title);
    
    m_listWidget = new QListWidget(this);
    connect(m_listWidget, &QListWidget::itemClicked, 
            this, &RepositoryView::onItemClicked);
    layout->addWidget(m_listWidget);
    
    m_addButton = new QPushButton(tr("Add Repository"), this);
    connect(m_addButton, &QPushButton::clicked, 
            this, &RepositoryView::onAddClicked);
    layout->addWidget(m_addButton);
    
    loadRepositories();
}

void RepositoryView::addRepository(const QString &path)
{
    // Check if already exists
    for (int i = 0; i < m_listWidget->count(); i++) {
        if (m_listWidget->item(i)->data(Qt::UserRole).toString() == path) {
            return;
        }
    }
    
    QFileInfo info(path);
    QListWidgetItem *item = new QListWidgetItem(info.fileName());
    item->setData(Qt::UserRole, path);
    item->setToolTip(path);
    m_listWidget->addItem(item);
    
    saveRepositories();
}

void RepositoryView::removeRepository(const QString &path)
{
    for (int i = 0; i < m_listWidget->count(); i++) {
        if (m_listWidget->item(i)->data(Qt::UserRole).toString() == path) {
            delete m_listWidget->takeItem(i);
            break;
        }
    }
    
    saveRepositories();
}

QStringList RepositoryView::repositories() const
{
    QStringList result;
    for (int i = 0; i < m_listWidget->count(); i++) {
        result << m_listWidget->item(i)->data(Qt::UserRole).toString();
    }
    return result;
}

void RepositoryView::onItemClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    emit repositorySelected(path);
}

void RepositoryView::onAddClicked()
{
    // This will be handled by the main window
    emit repositorySelected(QString());
}

void RepositoryView::loadRepositories()
{
    Settings settings;
    QStringList repos = settings.recentRepositories();
    
    for (const QString &path : repos) {
        QFileInfo info(path);
        if (info.exists()) {
            QListWidgetItem *item = new QListWidgetItem(info.fileName());
            item->setData(Qt::UserRole, path);
            item->setToolTip(path);
            m_listWidget->addItem(item);
        }
    }
}

void RepositoryView::saveRepositories()
{
    Settings settings;
    settings.setRecentRepositories(repositories());
}
