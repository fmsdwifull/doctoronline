#include "lujiazui.h"
#include <QtGui/QApplication>


#include <QTextCodec>
#include <QDebug>
#include <QByteArray>

#include <common.h>


SipInfo  usrAuth; //用户信息


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec::setCodecForTr(QTextCodec::codecForName(QTextCodec::codecForLocale()->name()));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(QTextCodec::codecForLocale()->name())); 
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName(QTextCodec::codecForLocale()->name()));

    // 应用程序单例检查  
    if(assumeSingleInstance("lujiazui") < 0)  
    {  
        QMessageBox::information(NULL,"提示",  "您好，程序已经在运行！");  
        return -1;  
    }  
	Qt::WindowFlags flags = 0;
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;

	ReadUserConfig();
	
	loginDlg login;
	if(login.exec()==QDialog::Accepted)
    {
		lujiazui w;
		w.show();
		RegisterSip();
        return a.exec();
    }
    else return 0;
	return a.exec();
}
