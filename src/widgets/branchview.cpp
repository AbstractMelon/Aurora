#include "branchview.h"
#include "git/gitrepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>

BranchView::BranchView(QWidget *parent)
    : QWidget(parent)
    , m_repository(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    
    QLabel *title = new QLabel(tr("Branches"), this);
    QFont font = title->font();
    font.setPointSize(14);
    font.setBold(true);
    title->setFont(font);
    mainLayout->addWidget(title);
    
    // Button bar
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_createButton = new QPushButton(tr("New Branch"), this);
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_checkoutButton = new QPushButton(tr("Checkout"), this);
    
    connect(m_createButton, &QPushButton::clicked, this, &BranchView::onCreateBranch);
    connect(m_deleteButton, &QPushButton::clicked, this, &BranchView::onDeleteBranch);
    connect(m_checkoutButton, &QPushButton::clicked, this, &BranchView::onCheckoutBranch);
    
    buttonLayout->addWidget(m_createButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addWidget(m_checkoutButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Branch tree
    m_branchTree = new QTreeWidget(this);
    m_branchTree->setHeaderLabels({tr("Branch Name"), tr("Type")});
    m_branchTree->setColumnWidth(0, 250);
    m_branchTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_branchTree, &QTreeWidget::customContextMenuRequested,
            this, &BranchView::onContextMenu);
    
    mainLayout->addWidget(m_branchTree);
}

void BranchView::setRepository(std::shared_ptr<GitRepository> repo)
{
    m_repository = repo;
    refresh();
}

void BranchView::refresh()
{
    if (!m_repository) return;
    
    updateBranchList();
}

void BranchView::updateBranchList()
{
    m_branchTree->clear();
    
    if (!m_repository) return;
    
    auto branches = m_repository->getBranches();
    QString currentBranch = m_repository->currentBranch();
    
    for (const auto &branch : branches) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_branchTree);
        
        QString displayName = branch.name;
        if (branch.isHead) {
            displayName += " *";
            QFont font = item->font(0);
            font.setBold(true);
            item->setFont(0, font);
        }
        
        item->setText(0, displayName);
        item->setText(1, branch.isRemote ? tr("Remote") : tr("Local"));
        item->setData(0, Qt::UserRole, branch.name);
    }
}

void BranchView::onCreateBranch()
{
    if (!m_repository) return;
    
    bool ok;
    QString name = QInputDialog::getText(this, tr("Create Branch"),
                                        tr("Branch name:"), QLineEdit::Normal,
                                        QString(), &ok);
    
    if (ok && !name.isEmpty()) {
        try {
            m_repository->createBranch(name);
            refresh();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("Error"), 
                tr("Failed to create branch: %1").arg(e.what()));
        }
    }
}

void BranchView::onDeleteBranch()
{
    if (!m_repository) return;
    
    auto selected = m_branchTree->selectedItems();
    if (selected.isEmpty()) return;
    
    QString branchName = selected.first()->data(0, Qt::UserRole).toString();
    
    auto reply = QMessageBox::question(this, tr("Delete Branch"),
        tr("Are you sure you want to delete branch '%1'?").arg(branchName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        try {
            m_repository->deleteBranch(branchName);
            refresh();
        } catch (const std::exception &e) {
            QMessageBox::critical(this, tr("Error"), 
                tr("Failed to delete branch: %1").arg(e.what()));
        }
    }
}

void BranchView::onCheckoutBranch()
{
    if (!m_repository) return;
    
    auto selected = m_branchTree->selectedItems();
    if (selected.isEmpty()) return;
    
    QString branchName = selected.first()->data(0, Qt::UserRole).toString();
    
    try {
        m_repository->checkoutBranch(branchName);
        refresh();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, tr("Error"), 
            tr("Failed to checkout branch: %1").arg(e.what()));
    }
}

void BranchView::onContextMenu(const QPoint &pos)
{
    QTreeWidgetItem *item = m_branchTree->itemAt(pos);
    if (!item) return;
    
    QMenu menu(this);
    menu.addAction(tr("Checkout"), this, &BranchView::onCheckoutBranch);
    menu.addAction(tr("Delete"), this, &BranchView::onDeleteBranch);
    
    menu.exec(m_branchTree->mapToGlobal(pos));
}
