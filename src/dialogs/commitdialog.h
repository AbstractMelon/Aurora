#ifndef COMMITDIALOG_H
#define COMMITDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <memory>

class GitRepository;

class CommitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommitDialog(std::shared_ptr<GitRepository> repo, QWidget *parent = nullptr);
    
    QString getMessage() const;
    QString getAuthorName() const;
    QString getAuthorEmail() const;
    QDateTime getAuthorDate() const;
    bool useCustomAuthor() const;
    bool useCustomDate() const;

private slots:
    void onValidate();
    void onCommit();
    void onAdvancedToggled(bool checked);

private:
    void loadChangedFiles();
    void setupAdvancedOptions();
    
    std::shared_ptr<GitRepository> m_repository;
    QTextEdit *m_messageEdit;
    QListWidget *m_fileList;
    QPushButton *m_commitButton;
    QPushButton *m_cancelButton;
    
    // Advanced options
    QCheckBox *m_advancedCheckBox;
    QGroupBox *m_advancedGroup;
    QCheckBox *m_customAuthorCheck;
    QLineEdit *m_authorNameEdit;
    QLineEdit *m_authorEmailEdit;
    QCheckBox *m_customDateCheck;
    QDateTimeEdit *m_dateTimeEdit;
};

#endif // COMMITDIALOG_H
