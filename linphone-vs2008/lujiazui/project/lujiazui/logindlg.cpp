#include "logindlg.h"
#include "common.h"

LoginInfo  lginfo; //用户信息
QString loginReturn;
extern SipInfo  usrAuth; //用户信息

loginDlg::loginDlg(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    Qt::WindowFlags flags = 0;

    //设置窗口图标
	QIcon icon;
	icon.addFile("image/logox.ico");
	setWindowIcon(icon);

    flags |= Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags); // 设置禁止最大化
    setFixedSize(460,300); // 禁止改变窗口大小。
	ui.pwdLineEdit->setEchoMode (QLineEdit::Password);

	//请求登陆信息
	loginRequest = new QNetworkAccessManager(this);
    connect(loginRequest, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyLoginFinished(QNetworkReply*)));
	connect(ui.cancelBtn,SIGNAL(clicked()),this,SLOT(close()));
}

loginDlg::~loginDlg()
{

}

void loginDlg::on_loginBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	QString relativePath(tr("/OAapp/WebObjects/OAapp.woa?cjApply=serviceLogin&username="));
	QString userPara,secretPara,codePara,keyPara;
	userPara=ui.usrLineEdit->text();
	lginfo.username=userPara;
	secretPara=ui.pwdLineEdit->text();
	lginfo.secret=secretPara;
	codePara=tr("2");

	//这里应该有隐患，如果终端时间和服务器日期时间不一样的话，请求会失败
	QDateTime time = QDateTime::currentDateTime().addDays(0);;//获取系统现在的时间
    QString dateKeyStr = time.toString("yyyy-MM-dd"); //设置显示格式
    keyPara=getMd5Check(dateKeyStr+tr("LOGIN"));
	QString requestUrl=ipStr+relativePath+userPara+tr("&userpass=")+secretPara+tr("&Sta=")+codePara+tr("&checkstr=")+keyPara;
	qDebug()<<requestUrl;
    loginRequest->get(QNetworkRequest(QUrl(requestUrl)));
}


//获取医生分类列表
void loginDlg::replyLoginFinished(QNetworkReply *reply)
{
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
	loginReturn=all;

	if(loginReturn==tr("-1"))
	{
		QMessageBox::warning(this,tr("Warning"),tr("user name or password error!"),QMessageBox::Yes);
	}else{
		QFile file("loginInfo.json");  
		if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
		{  
			qDebug()<< "Open failed......" ;  
			return;  
		}  
		QTextStream txtOutput(&file);  
		txtOutput << all << endl;  
		file.close(); 
		accept();
	}
}