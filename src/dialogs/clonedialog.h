#ifndef CLONEDIALOG_H
#define CLONEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class CloneDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    
    QString getUrl() const;
    QString getPath() const;

private slots:
    void onBrowse();
    void onValidate();

private:
    QLineEdit *m_urlEdit;
    QLineEdit *m_pathEdit;
    QPushButton *m_browseButton;
    QPushButton *m_cloneButton;
    QPushButton *m_cancelButton;
};

#endif // CLONEDIALOG_H
