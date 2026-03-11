#include "clonedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFileDialog>

CloneDialog::CloneDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Clone Repository"));
    setMinimumWidth(500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Form layout
    QFormLayout *formLayout = new QFormLayout();
    
    m_urlEdit = new QLineEdit(this);
    m_urlEdit->setPlaceholderText(tr("https://github.com/user/repo.git"));
    connect(m_urlEdit, &QLineEdit::textChanged, this, &CloneDialog::onValidate);
    formLayout->addRow(tr("Repository URL:"), m_urlEdit);
    
    QHBoxLayout *pathLayout = new QHBoxLayout();
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setPlaceholderText(tr("/path/to/clone/destination"));
    connect(m_pathEdit, &QLineEdit::textChanged, this, &CloneDialog::onValidate);
    
    m_browseButton = new QPushButton(tr("Browse..."), this);
    connect(m_browseButton, &QPushButton::clicked, this, &CloneDialog::onBrowse);
    
    pathLayout->addWidget(m_pathEdit);
    pathLayout->addWidget(m_browseButton);
    formLayout->addRow(tr("Local Path:"), pathLayout);
    
    mainLayout->addLayout(formLayout);
    
    // Button box
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    m_cloneButton = new QPushButton(tr("Clone"), this);
    m_cloneButton->setEnabled(false);
    m_cloneButton->setDefault(true);
    connect(m_cloneButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_cloneButton);
    
    mainLayout->addLayout(buttonLayout);
}

QString CloneDialog::getUrl() const
{
    return m_urlEdit->text();
}

QString CloneDialog::getPath() const
{
    return m_pathEdit->text();
}

void CloneDialog::onBrowse()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Destination"));
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void CloneDialog::onValidate()
{
    bool valid = !m_urlEdit->text().isEmpty() && !m_pathEdit->text().isEmpty();
    m_cloneButton->setEnabled(valid);
}
