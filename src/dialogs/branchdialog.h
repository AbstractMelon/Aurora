#ifndef BRANCHDIALOG_H
#define BRANCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <memory>

class GitRepository;

class BranchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BranchDialog(std::shared_ptr<GitRepository> repo, QWidget *parent = nullptr);
    
    QString getBranchName() const;
    QString getStartPoint() const;

private slots:
    void onValidate();

private:
    std::shared_ptr<GitRepository> m_repository;
    QLineEdit *m_nameEdit;
    QComboBox *m_startPointCombo;
    QPushButton *m_createButton;
    QPushButton *m_cancelButton;
};

#endif // BRANCHDIALOG_H
