#ifndef VASCLICKLABEL_H
#define VASCLICKLABEL_H
#include <QLabel>  
  
  
/************************************************* 
Class Name�� VASClickLabel 
Description: ���е�����Ӧ�ı�ǩ�� 
*************************************************/  
class VASClickLabel : public QLabel  
{  
    Q_OBJECT  
public:  
    explicit VASClickLabel(QWidget *parent = 0);  
    VASClickLabel(const QString &text, QWidget *parent=0);  
signals:  
    // ��굥���ź�  
    void clicked();//IMClickLabel* label);  
  
protected:  
    // ��굥���¼�  
    void mouseReleaseEvent(QMouseEvent *);  
    void enterEvent(QEvent *);  
    void leaveEvent(QEvent *);  
public slots:  
      
};  
  
#endif // VASCLICKLABEL_H  