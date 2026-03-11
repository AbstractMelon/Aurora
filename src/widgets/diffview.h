#ifndef DIFFVIEW_H
#define DIFFVIEW_H

#include <QWidget>

class DiffView : public QWidget
{
    Q_OBJECT

public:
    explicit DiffView(QWidget *parent = nullptr);
};

#endif // DIFFVIEW_H
