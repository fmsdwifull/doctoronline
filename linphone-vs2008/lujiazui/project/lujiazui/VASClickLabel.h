#ifndef VASCLICKLABEL_H
#define VASCLICKLABEL_H
#include <QLabel>  
  
  
/************************************************* 
Class Name： VASClickLabel 
Description: 具有单击相应的标签类 
*************************************************/  
class VASClickLabel : public QLabel  
{  
    Q_OBJECT  
public:  
    explicit VASClickLabel(QWidget *parent = 0);  
    VASClickLabel(const QString &text, QWidget *parent=0);  
signals:  
    // 鼠标单击信号  
    void clicked();//IMClickLabel* label);  
  
protected:  
    // 鼠标单击事件  
    void mouseReleaseEvent(QMouseEvent *);  
    void enterEvent(QEvent *);  
    void leaveEvent(QEvent *);  
public slots:  
      
};  
  
#endif // VASCLICKLABEL_H  