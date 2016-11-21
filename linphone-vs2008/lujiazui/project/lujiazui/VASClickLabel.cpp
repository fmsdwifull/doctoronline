#include "VASClickLabel.h"  
#include <QMouseEvent>  
  
// mark: public:--------------------------------------------------  
VASClickLabel::VASClickLabel(QWidget *parent) :  
    QLabel(parent)  
{  
    QPalette pa;  
    pa.setColor(QPalette::WindowText, Qt::darkGreen);  
    setPalette(pa);  
}  
  
VASClickLabel::VASClickLabel(const QString &text, QWidget *parent) :  
    QLabel(parent)  
{  
    setText(text);  
}  
  
// mark: protected:------------------------------------------------  
void VASClickLabel::mouseReleaseEvent(QMouseEvent *ev)  
{  
    if (ev->button() == Qt::LeftButton)  
        emit clicked();//(this);  
}  
  
void VASClickLabel::enterEvent(QEvent * )  
{  
    QPalette pa;  
    pa.setColor(QPalette::WindowText, Qt::blue);  
    setPalette(pa);  
}  
  
void VASClickLabel::leaveEvent(QEvent *)  
{  
    QPalette pa;  
    pa.setColor(QPalette::WindowText, Qt::darkGreen);  
    setPalette(pa);  
} 