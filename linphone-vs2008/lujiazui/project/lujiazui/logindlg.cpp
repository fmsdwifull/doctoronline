#include "logindlg.h"
#include "common.h"

LoginInfo  lginfo; //�û���Ϣ
QString loginReturn;
extern SipInfo  usrAuth; //�û���Ϣ

loginDlg::loginDlg(QDialog *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
    Qt::WindowFlags flags = 0;

    //���ô���ͼ��
	QIcon icon;
	icon.addFile("image/logox.ico");
	setWindowIcon(icon);

    flags |= Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags); // ���ý�ֹ���
    setFixedSize(460,300); // ��ֹ�ı䴰�ڴ�С��
	ui.pwdLineEdit->setEchoMode (QLineEdit::Password);

	//�����½��Ϣ
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

	//����Ӧ��������������ն�ʱ��ͷ���������ʱ�䲻һ���Ļ��������ʧ��
	QDateTime time = QDateTime::currentDateTime().addDays(0);;//��ȡϵͳ���ڵ�ʱ��
    QString dateKeyStr = time.toString("yyyy-MM-dd"); //������ʾ��ʽ
    keyPara=getMd5Check(dateKeyStr+tr("LOGIN"));
	QString requestUrl=ipStr+relativePath+userPara+tr("&userpass=")+secretPara+tr("&Sta=")+codePara+tr("&checkstr=")+keyPara;
	qDebug()<<requestUrl;
    loginRequest->get(QNetworkRequest(QUrl(requestUrl)));
}


//��ȡҽ�������б�
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