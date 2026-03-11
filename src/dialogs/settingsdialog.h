#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();
    void onCancel();

private:
    void setupGeneralTab();
    void setupGitTab();
    void setupGitHubTab();
    void loadSettings();
    void saveSettings();
    
    QTabWidget *m_tabWidget;
    
    // General settings
    QLineEdit *m_editorEdit;
    
    // Git settings
    QLineEdit *m_userNameEdit;
    QLineEdit *m_userEmailEdit;
    
    // GitHub settings
    QLineEdit *m_githubTokenEdit;
    
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
};

#endif // SETTINGSDIALOG_H
