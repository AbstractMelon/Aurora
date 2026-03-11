#include "branchdialog.h"
#include "git/gitrepository.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

BranchDialog::BranchDialog(std::shared_ptr<GitRepository> repo, QWidget *parent)
    : QDialog(parent)
    , m_repository(repo)
{
    setWindowTitle(tr("Create Branch"));
    setMinimumWidth(400);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Form layout
    QFormLayout *formLayout = new QFormLayout();
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("feature/new-feature"));
    connect(m_nameEdit, &QLineEdit::textChanged, this, &BranchDialog::onValidate);
    formLayout->addRow(tr("Branch Name:"), m_nameEdit);
    
    m_startPointCombo = new QComboBox(this);
    m_startPointCombo->addItem(tr("Current Branch (HEAD)"), QString());
    
    if (m_repository) {
        auto branches = m_repository->branches();
        for (const QString &branch : branches) {
            m_startPointCombo->addItem(branch, branch);
        }
    }
    
    formLayout->addRow(tr("Start Point:"), m_startPointCombo);
    
    mainLayout->addLayout(formLayout);
    
    // Button box
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    m_createButton = new QPushButton(tr("Create"), this);
    m_createButton->setEnabled(false);
    m_createButton->setDefault(true);
    connect(m_createButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_createButton);
    
    mainLayout->addLayout(buttonLayout);
}

QString BranchDialog::getBranchName() const
{
    return m_nameEdit->text();
}

QString BranchDialog::getStartPoint() const
{
    return m_startPointCombo->currentData().toString();
}

void BranchDialog::onValidate()
{
    bool valid = !m_nameEdit->text().isEmpty();
    m_createButton->setEnabled(valid);
}
