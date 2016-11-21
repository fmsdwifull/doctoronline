#include "lujiazui.h"
#include <QDateTime>
#include <QDesktopServices>

#define PHONE_CORE_TIMER 100

static LinphoneCoreVTable vTable;
static const char *config_file = "siplog.ini";
static const char *factoryConfig = "siplog.ini";
LinphoneCore *the_core=NULL;

extern SipInfo  usrAuth; //�û���Ϣ
extern LoginInfo  lginfo; //�û���Ϣ

//���������������
QMap<QString,DoctoryInfo>  map_docinfo_sip;//currentCallTel

static void linphone_log_handler(OrtpLogLevel lev, const char *fmt, va_list args)
{
	const char *lname="undef";
	char *msg;
	char *msg_str;

	switch(lev)
	{
			case ORTP_DEBUG:
				lname="DEBUG";
				break;
			case ORTP_MESSAGE:
				lname="MESSAGE";
				break;
			case ORTP_WARNING:
				lname="WARING";
				break;
			case ORTP_ERROR:
				lname="ERROR";
				break;
			case ORTP_FATAL:
				lname="FATAL";
				break;
			default:
				lname = ("Bad level !");
	}
	msg=ortp_strdup_vprintf(fmt,args);
	msg_str = ortp_strdup_printf("%s: %s\r\n",lname,msg);
#ifdef _MSC_VER
//	OutputDebugString(msg_str);
#endif
	ms_free(msg);
	ms_free(msg_str);
}

//���磬�򵯳�����Ի���
static void showInterfaceCb(LinphoneCore *lc)
{

}
static  void inviteReceivedCb(LinphoneCore *lc, const char *from) 
{
	
}

static void byeReceivedCb(LinphoneCore *lc, const char *from) 
{

}

static void displayStatusCb(LinphoneCore *lc, const char *message) 
{
	lujiazui *the_ui=(lujiazui*)linphone_core_get_user_data(lc);
	QString msg(message);
	qDebug()<<msg;
	QString serverId;
	bool ret1=msg.contains("Call terminated");
	bool ret2=msg.contains("Call ended");
	bool ret3=msg.contains("Connected");
	bool ret4=msg.contains("Registration");
	bool ret5=msg.contains("Contacting");
	bool ret6=msg.contains("busy");
	bool ret7=msg.contains("successful");
	if(ret4 && ret7)
	{
		the_ui->ui.callStateLabel->setText("ע��ɹ�");
	}
	if(ret5)
	{
		the_ui->ui.callStateLabel->setText("���ں���");
	}
	if(ret6)
	{
		the_ui->ui.callStateLabel->setText("�޷���ͨ");
		the_ui->callNums++;
	
	}
	if(ret1||ret2)
	{
		the_ui->ui.callStateLabel->setText("ע��ɹ�");
		the_ui->isServicing=false;
		the_ui->scollText=QString(" �������������");
		the_ui->ui.scolltextLabel->setText(the_ui->scollText);
		if(!the_ui->scollTimer->isActive())
		{
			the_ui->scollTimer->start(1000);
			the_ui->scollText=QString(" ��л��������������Ͻ����´η��񵹼�ʱ��");
			the_ui->ui.scolltextLabel->setText(the_ui->scollText);
		}
		for( int index=0;index < the_ui->docInfoVector.size(); index++)
		{
			if(the_ui->currentCallTel==QString(the_ui->docInfoVector.at(index).siptel))
			{
				serverId=QString(the_ui->docInfoVector.at(index).id);
				break;
			}
		}
		if(!serverId.isEmpty())
			the_ui->finishiedServicer(serverId);

		//�رս���
		int size=the_ui->docInfoVector.count();
		if(size > 0)
		{
			QString ipStr(usrAuth.serverip);
			QString deviceId=the_ui->docInfoVector.at(0).deviceId;
			QString tmpClinetStr1(QObject::tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_push&title=vid_before&mes="));
			QString tmpClinetStr2(QObject::tr("close"));
			QString tmpClinetStr3(QObject::tr("&deviceId="));
			qDebug()<<ipStr+tmpClinetStr1+tmpClinetStr2+tmpClinetStr3+deviceId;
			the_ui->pushClientRequest->get(QNetworkRequest(QUrl(ipStr+tmpClinetStr1+tmpClinetStr2+tmpClinetStr3+deviceId)));
		}
	}
	if(ret3)
	{
		the_ui->ui.callStateLabel->setText("����ͨ��");
		the_ui->servStartDt=QDateTime::currentDateTime().addSecs(60*20);
		the_ui->isServicing=true;
		the_ui->countMins=0;
		the_ui->scollText=QString(" �����������ڽ�����...");
		the_ui->ui.scolltextLabel->setText(the_ui->scollText);
		the_ui->scollTimer->stop();

		int size=the_ui->docInfoVector.count();
		if(size > 0)
		{
			QString ipStr(usrAuth.serverip);
			QString deviceId=the_ui->docInfoVector.at(0).deviceId;
			QString tmpClinetStr1(QObject::tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_push&title=vid_before&mes="));
			QString tmpClinetStr2(QObject::tr("close"));
			QString tmpClinetStr3(QObject::tr("&deviceId="));
			qDebug()<<ipStr+tmpClinetStr1+tmpClinetStr2+tmpClinetStr3+deviceId;
			the_ui->pushClientRequest->get(QNetworkRequest(QUrl(ipStr+tmpClinetStr1+tmpClinetStr2+tmpClinetStr3+deviceId)));

			QString tmpClinetStr4(QObject::tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_push&title=vid_after&mes="));
			QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat);   
			QString tmpClinetStr5 = configIniRead->value("/settings/servicetime").toString(); 
			delete configIniRead; 
			//QString tmpClinetStr5(QObject::tr("4"));
			QString tmpClinetStr6(QObject::tr("&deviceId="));
			qDebug()<<ipStr+tmpClinetStr4+tmpClinetStr5+tmpClinetStr6+deviceId;
			the_ui->pushClientRequest->get(QNetworkRequest(QUrl(ipStr+tmpClinetStr4+tmpClinetStr5+tmpClinetStr6+deviceId)));
		}
		//����ͨ����0
		the_ui->callNums=0;
		the_ui->iscalled=true;
	}

}

static void displayMessageCb(LinphoneCore *lc, const char *message) 
{
	QString path = QDir::currentPath();
}
static void authInfoRequested(LinphoneCore *lc, const char *realm, const char *username) 
{
}
static void generalStateChange(LinphoneCore *lc, LinphoneGeneralState *gstate) 
{
}

static void network_quality(LinphoneCore *lc, int ms)
{
}

static void media_quality(LinphoneCore *lc, int jitter, float packet_lost)
{
}

lujiazui::lujiazui(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);

	//��ʼ��linphonelib
	linphone_core_enable_logs_with_cb(linphone_log_handler);
	memset(&vTable,0,sizeof(vTable));
	vTable.show = showInterfaceCb;
	vTable.inv_recv = inviteReceivedCb;
	vTable.auth_info_requested = authInfoRequested;
	vTable.display_status = displayStatusCb;
	vTable.display_message = displayMessageCb;
	vTable.display_warning = displayMessageCb;
	vTable.general_state = generalStateChange;
	vTable.network_quality_monitor = network_quality;
	vTable.media_quality_monitor = media_quality;

	the_core = linphone_core_new(	&vTable
		,config_file
		,factoryConfig
		,this);

	linphone_core_set_playback_gain_db(the_core,1.0);

    //���ô���ͼ��
	QIcon icon;
	icon.addFile("image/logox.ico");
	setWindowIcon(icon);

	//����󻯰�ť
	setWindowFlags(Qt::Widget);
	oldItems=0;
	newItems=0;
	pos=0;
	isServicing=false;
	countMins=0;
	callNums=0;
	sendNums=0;
	iscalled=false;
	
	//linphone����ѭ����ʱ��
	QTimer *timer = new QTimer();
	connect(timer,SIGNAL(timeout()),this,SLOT(linponeIterate()));
	timer->start(30); 

	//�б�ˢ�¶�ʱ
	QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat);  
	int updatelisttime = configIniRead->value("/settings/updatelisttime").toString().toInt(); 
	delete configIniRead;  
	updateTimer = new QTimer();
	connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateAppoitment()));
	updateTimer->start(1000*60*updatelisttime); 

	//������Ļˢ�¶�ʱ��
	scollTimer=new QTimer();
	connect(scollTimer,SIGNAL(timeout()),this,SLOT(scollTimerOut()));
	scollTimer->start(1000); 
	//
	//����ҽ��json�б�
	managerDoctory = new QNetworkAccessManager(this);
    connect(managerDoctory, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyDocFinished(QNetworkReply*)));

	//�޸ķ���״̬
	checkState = new QNetworkAccessManager(this);
    connect(checkState, SIGNAL(finished(QNetworkReply*)),this, SLOT(replyCSFinished(QNetworkReply*)));
    
	//������Ϣ����
	sendInfoRequest= new QNetworkAccessManager(this);
    connect(sendInfoRequest, SIGNAL(finished(QNetworkReply*)),this, SLOT(sendIFFinished(QNetworkReply*))); 

	//��ͻ���������Ϣ
	pushClientRequest = new QNetworkAccessManager(this);
	connect(pushClientRequest, SIGNAL(finished(QNetworkReply*)),this, SLOT(pushClientFinished(QNetworkReply*))); 

	//ҽ���б���ʼ��
	initModel=new QStandardItemModel(100, 6, this);
	ui.tableView->setEditTriggers ( QAbstractItemView::NoEditTriggers );
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);	
	ui.tableView->setModel(initModel);
	ui.tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch) ;
	ui.tableView->verticalHeader()->hide();  //setViewportMargins
	initModel->setHeaderData(0, Qt::Horizontal, tr("������"));
	ui.tableView->setColumnHidden(0,true);
	initModel->setHeaderData(1, Qt::Horizontal, tr("��������"));
	initModel->setHeaderData(2, Qt::Horizontal, tr("��ϵ�绰"));
	initModel->setHeaderData(3, Qt::Horizontal, tr("ԤԼ����"));
    initModel->setHeaderData(4, Qt::Horizontal, tr("ԤԼʱ��"));
	ui.tableView->setColumnHidden(4,true);
	initModel->setHeaderData(5, Qt::Horizontal, tr("����ʱ��"));
	ui.tableView->setColumnHidden(5,true);
	
	//����ԤԼ����
	QString keyIndex=getLoginId();
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	qDebug()<<ipStr+tmpStr1+keyIndex;
    managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr1+keyIndex)));

	// ��ȡ��ͼ����Ŀѡ��ģ��
    QItemSelectionModel *selectionModel = ui.tableView->selectionModel();
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),this, SLOT(changeCurrent(QModelIndex,QModelIndex)));

	//������ťĬ�ϲ���ʹ��
	//ui.serviceCsBtn->setEnabled(false);
	//ui.usernaneLabel->setText(tr("  �û���")+lginfo.username); //
    QString linkStr(tr("/OAapp/WebObjects/OAapp.woa"));;

	ui.usernaneLabel->setText(tr("  ��½�� ")+tr("<a href ='")+ipStr+linkStr+tr("'>")+lginfo.username+tr("</a>"));;
	ui.h_id_label->hide();

	//�������̲˵�
	//createActions();
	//setOpenExternalLinks = true;
	//QDesktopServices::openUrl(QUrl("www.baidu.com"));  //openUrl������ʵ��
	ui.usernaneLabel->setOpenExternalLinks(true);

    

}

lujiazui::~lujiazui()
{

}

void lujiazui::linponeIterate()
{
	 if (the_core != NULL)
	 {
		linphone_core_iterate(the_core);
	 }
}

void lujiazui::on_sipcallBtn_clicked()
{
	char m_address[64];
	QString tmpStr=ui.lineEditTest->text();
	if(!tmpStr.isEmpty())
	{
		sprintf(m_address,"sip:%s@%s",tmpStr.toStdString().c_str(),usrAuth.sipip);
		//QMessageBox::StandardButton rb = QMessageBox::question(NULL, tr("��ܰ��ʾ"), tr("�����ں���")+tr(m_address)+tr(",��ȷ�ϣ�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//QMessageBox::StandardButton rb;		
		int ret = QMessageBox::information(NULL, tr("��ܰ��ʾ"), tr("�����ں���")+tr(m_address)+tr(",��ȷ�ϣ�"), tr("ȷ��"), tr("ȡ��"));
		if(0==ret)
		//if(rb == QMessageBox::Yes)
		{
			LinphoneProxyConfig* proxyCfg=NULL;	
			linphone_core_get_default_proxy(the_core,&proxyCfg);
			if(proxyCfg!=NULL)
			{
				linphone_core_invite(the_core,m_address) ;
			}
			currentCallTel=tmpStr;
		}
	}else{
		QMessageBox::warning(NULL,"��ܰ��ʾ��","��û��ѡ�������������ѡ�����");
	}
}

void lujiazui::on_siphungupBtn_clicked()
{
	linphone_core_terminate_call(the_core,NULL);
}

void lujiazui::on_listUpdateBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	QString keyIndex=getLoginId();
	qDebug()<<ipStr+tmpStr1+keyIndex;
    managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr1+keyIndex)));
}

//������״̬�޸İ�ť,�������
void lujiazui::on_serviceCsBtn_clicked()
{
	QString ipStr(usrAuth.serverip);
	QString serviceID=ui.h_id_label->text();
	QString todayStr;
	QDateTime time = QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
	todayStr = time.toString("yyyy-MM-dd"); //������ʾ��ʽ
	QString md5Str=getMd5Check(todayStr+serviceID);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_status&handle=over&id="));
	QString pathStr1=tr("&checkstr=");
	qDebug()<<ipStr+tmpStr1+serviceID+pathStr1+md5Str;
    checkState->get(QNetworkRequest(QUrl(ipStr+tmpStr1+serviceID+pathStr1+md5Str)));
	
	//ˢ���б�
	QString keyIndex=getLoginId();
	QString tmpStr2(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	qDebug()<<ipStr+tmpStr1+keyIndex;
    managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr2+keyIndex)));
}

//�˳���¼
void lujiazui::on_signoutBtn_clicked()
{
	this->hide();
	loginDlg login;
	if(login.exec()==QDialog::Accepted)
    {
        ui.usernaneLabel->setText(tr("��½����")+lginfo.username);
		QString ipStr(usrAuth.serverip);
		QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
		QString keyIndex=getLoginId();
		qDebug()<<ipStr+tmpStr1+keyIndex;
		managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr1+keyIndex)));
		this->show();
    }
}

//��ͻ��ˣ������У�����������Ϣ
void lujiazui::pushClientFinished(QNetworkReply *reply)
{
	 QTextCodec *codec = QTextCodec::codecForName("UTF-8");
     QString all = codec->toUnicode(reply->readAll());
}


void lujiazui::sendIFFinished(QNetworkReply *reply)
{
	 QTextCodec *codec = QTextCodec::codecForName("UTF-8");
     QString all = codec->toUnicode(reply->readAll());
}

//�޸ķ���״̬
void lujiazui::replyCSFinished(QNetworkReply *reply)
{
	 QTextCodec *codec = QTextCodec::codecForName("UTF-8");
     QString all = codec->toUnicode(reply->readAll());
	 qDebug()<<"over state is : "<<all;

	 //Ϊ�´η����ʼ��һЩ����
	 docInfoVector.clear();	
	 qDebug()<<"for next service  readed";
	 iscalled=false;
	 sendNums=0;

	 //ˢ���б�
	 QString ipStr(usrAuth.serverip);
	 QString keyIndex=getLoginId();
	 QString tmpStr2(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	 qDebug()<<ipStr+tmpStr2+keyIndex;
	 managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr2+keyIndex)));
}

//��ȡҽ���б�
void lujiazui::replyDocFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString all = codec->toUnicode(reply->readAll());
    QFile file("doctoryljz.json");  
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))  
    {  
        qDebug()<< "Open failed......" ;  
        return;  
    }  
    QTextStream txtOutput(&file);  
    txtOutput << all << endl;  
    file.close(); 
	loadDocJson();
	initDocTable();//
	countMins++;

	//����Ĭ���ڵ�һ��
	int size=docInfoVector.count();
	if(size > 0)
	{
		//QDateTime appointDatetime=QDateTime::fromString("2015-06-26 17:00", "yyyy-MM-dd hh:mm"); 
		QDateTime appointDatetime=QDateTime::fromString(docInfoVector.at(0).datetime, "yyyy-MM-dd hh:mm"); 
		QDateTime nowDatetime=QDateTime::currentDateTime();
		if(isServicing)
		{
			if(countMins>3)
			{
				scollTimer->start(1000); 
				countDownDt=getCountDownDt( nowDatetime,servStartDt);
				scollText=tr("   ��ã���������������")+countDownDt+tr("�����ע!");
			}
		}else{
			   //���񵹼�ʱ���ѣ��������Ѿ��Ź���
			   countDownDt=getCountDownDt( nowDatetime,appointDatetime);
			   scollText=tr("   ��ã��������ʼ����")+countDownDt+tr("�����ע!");
		}


		//����������ǰsendinfotm
		QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat);  
		int sendinfotm = configIniRead->value("/settings/sendinfotime").toString().toInt(); 
		int sericetm = configIniRead->value("/settings/servicetime").toString().toInt(); 
		delete configIniRead;  
		QDateTime sendinfoDatetime=QDateTime::currentDateTime().addSecs(60*sendinfotm);
		QDateTime invatedDatetime=QDateTime::currentDateTime().addSecs(-60*sericetm);
		if(!iscalled)
		{
			if(sendinfoDatetime>=appointDatetime)
			{
				if(sendNums==0)
				{
					qDebug()<<"start send info and message ";
					sendNums++;
					//���Ͷ�������
					QString ipStr(usrAuth.serverip);
					QString relativePath(tr("/OAapp/WebObjects/OAapp.woa?cjApply=smsnote&code="));
  					QDateTime time = QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
					QString dateKeyStr = time.toString("yyyy-MM-dd"); //������ʾ��ʽ
					QString keyPara=getMd5Check(dateKeyStr+tr("SMSN"));

					QString sendUrl=ipStr+relativePath+lginfo.username+tr("&checkstr=")+keyPara+tr("&recordid=")+docInfoVector.at(0).id;
					qDebug()<<sendUrl;
					sendInfoRequest->get(QNetworkRequest(sendUrl));

					//�ͻ��˷�����ʾ��Ϣ���ӿ�http://222.73.215.182:88/OAapp/WebObjects/OAapp.woa?cjApply=doctor_push&mes=&deviceId
					//ȡ����һ��
					QString deviceId=docInfoVector.at(0).deviceId;
					QString tmpClinetStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_push&title=vid_before"));
					//QString tmpClinetStr2(tr(""));
					QString tmpClinetStr2(tr("&deviceId="));
					qDebug()<<ipStr+tmpClinetStr1+tmpClinetStr2+deviceId;
					pushClientRequest->get(QNetworkRequest(QUrl(ipStr+tmpClinetStr1+tmpClinetStr2+deviceId)));
				}
			}
		}

		//��ʱ�Զ����Ź���
		QDateTime callOverDatetime=appointDatetime.addSecs(60*6);
		
		qDebug()<<"�Զ�����"<<callNums;
		if((nowDatetime>=appointDatetime)&&(nowDatetime<=callOverDatetime)&&((callNums==0)||(callNums==3)||(callNums==6)))
		{
			//�Զ�����
			char sipAddress[60];
			if(!iscalled)
				if(strcmp(docInfoVector.at(0).siptel,"")!=0)
				{
					qDebug()<<"start call tel ";
					currentCallTel=QString(docInfoVector.at(0).siptel);
					sprintf(sipAddress,"sip:%s@%s",docInfoVector.at(0).siptel,usrAuth.sipip);
					LinphoneProxyConfig* proxyCfg=NULL;	
					linphone_core_get_default_proxy(the_core,&proxyCfg);
					if(proxyCfg!=NULL)
					{
						linphone_core_invite(the_core,sipAddress) ;
					}		
				}
		}
		if((1==callNums)||(2==callNums)||(3==callNums)||(4==callNums)||(5==callNums))
			callNums++;
		

		
      
		 
		//����ԤԼʱ��15���ӣ�ǿ�Ʒ������
		//qDebug()<<"wuxiao"<<invatedDatetime.toString("yyyy-MM-dd hh:mm");
		//qDebug()<<"yuyue"<<appointDatetime.toString("yyyy-MM-dd hh:mm");
		if(invatedDatetime>=appointDatetime)
		{
			QString serverId(docInfoVector.at(0).id);
			if(!serverId.isEmpty())
				finishiedServicer(serverId);
		}
	}else{
		scollText=tr("   ���˺�û��ԤԼ������Ŀ����л���Ĺ�ע��");
	}
}

void lujiazui::loadDocJson()
{
	// ����json��Json::Reader  
    Json::Reader reader;    
	std::ifstream file;  
	file.open ("doctoryljz.json");
    Json::Value root;
    Json::Value item;
    reader.parse(file,root);

	//��սṹ�����´����µ�����
	map_docinfo.clear();
	docInfoVector.clear();
	int size = root.size();
	DoctoryInfo dcinfo;
	for (int i=0; i<size; ++i)
    {
		QString service=QString::fromStdString(root[i]["service"].asString());
		strcpy(dcinfo.service,service.toStdString().c_str());//

		QString deviceId=QString::fromStdString(root[i]["deviceId"].asString());
		strcpy(dcinfo.deviceId,deviceId.toStdString().c_str());//

		QString datetime=QString::fromStdString(root[i]["datetime"].asString());
		strcpy(dcinfo.datetime,datetime.toStdString().c_str());//datetime
		QString siptel=QString::fromStdString(root[i]["SIP"].asString());
		strcpy(dcinfo.siptel,siptel.toStdString().c_str());
		QString yuyuedate=QString::fromStdString(root[i]["yuyuedate"].asString());
		strcpy(dcinfo.yuyuedate,yuyuedate.toStdString().c_str());
		QString yuyuetime=QString::fromStdString(root[i]["yuyuetime"].asString());
		strcpy(dcinfo.yuyuetime,yuyuetime.toStdString().c_str());
		QString id=QString::fromStdString(root[i]["id"].asString());
		strcpy(dcinfo.id,id.toStdString().c_str());
		QString senddt=QString::fromStdString(root[i]["senddt"].asString());
		strcpy(dcinfo.senddt,senddt.toStdString().c_str());
		map_docinfo.insert(dcinfo.id,dcinfo);
		docInfoVector.append(dcinfo);
		map_docinfo_sip.insert(dcinfo.siptel,dcinfo);//���������绰
    }
	newItems=map_docinfo.count();
	newItems=docInfoVector.count();
}

void lujiazui::initDocTable()
{
	if(oldItems!=newItems)
	{
		int rowNum=initModel->rowCount();
		initModel->removeRows(0,rowNum);
		initModel->insertRows(0,rowNum);
		int i=0;
		for( int index=0;index < docInfoVector.size(); index++,i++)
		{
			initModel->setData(initModel->index(i,0),docInfoVector.at(index).id);	
			initModel->setData(initModel->index(i,1),docInfoVector.at(index).service);		
			initModel->setData(initModel->index(i,2),docInfoVector.at(index).siptel);
			initModel->setData(initModel->index(i,3),docInfoVector.at(index).datetime);

            //������ʾʱ���ʽ          
  			QString left=QString(docInfoVector.at(index).yuyuetime) .mid(0,2);
			QString right=QString(docInfoVector.at(index).yuyuetime).mid(2,2);
			QString geshitime=left+tr(":")+right;
			initModel->setData(initModel->index(i,4),geshitime);
			initModel->setData(initModel->index(i,5),docInfoVector.at(index).senddt);
		}
		oldItems=docInfoVector.count();
	
	}
	ui.tableView->selectRow(0);
}


//ʱ��Աȷ���������Ϣ
int lujiazui::compareTime()
{
	return -1;

}

void lujiazui::comboBoxValueChanged(QString currentText)
{
	QMap<QString,DoctoryClassList>::const_iterator go_iter;
	int i=0;
	for( go_iter=map_docclass.constBegin(); go_iter!=map_docclass.constEnd(); go_iter++,i++)
	{
		if(currentText.compare(go_iter.value().c_name)==0){
			keyIndex=go_iter.value().c_code;
			break;
		}
	}
	keyIndex = keyIndex.mid(1); 
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	qDebug()<<ipStr+tmpStr1+keyIndex;
    managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr1+keyIndex)));
}

// �ı䵱ǰ��Ŀ
void lujiazui::changeCurrent(const QModelIndex &current,const QModelIndex &previous)
{
	int currentRow=previous.row();
	QString index=ui.tableView->model()->index(current.row(),0).data().toString();
	if(!index.isEmpty())
	{
		int i;
		for( i=0;i< docInfoVector.size();i++)
		{
			if(strcmp(docInfoVector.at(i).id,index.toStdString().c_str())==0)
			{
				ui.lineEditTest->setText(docInfoVector.at(i).siptel);
				ui.h_id_label->setText(docInfoVector.at(i).id);
				break;
			}
		}
	}
}

//��ȡ��½��Ϣ
QString lujiazui::getLoginId()
{
	// ����json��Json::Reader  
    Json::Reader reader;    
	std::ifstream file;  
	file.open ("loginInfo.json");
    Json::Value root;
    Json::Value item;
    reader.parse(file,root);
	
	QString serverId;
	QString usr;
	QString passwd;
	QString sipIp;
	int size = root.size();
	if(0!=size)
	{
		 serverId=QString::fromStdString(root["serverid"].asString());//SIPPASS
		 usr=QString::fromStdString(root["SIP"].asString());
		 passwd=QString::fromStdString(root["SIPPASS"].asString());
		 sipIp=QString::fromStdString(root["SIPIP"].asString());
	}

	//�������ļ�
	QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat); 
	configIniRead->setValue("/sipuser/sipIP",QVariant(sipIp));
	configIniRead->setValue("/sipuser/usr",QVariant(usr));
    configIniRead->setValue("/sipuser/pwd",QVariant(passwd));
	delete configIniRead;  
	
	return serverId;
}

//
void lujiazui::updateAppoitment()
{
	QString keyIndex=getLoginId();
	QString ipStr(usrAuth.serverip);
	QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_yuyue&serverid="));
	qDebug()<<ipStr+tmpStr1+keyIndex;
    managerDoctory->get(QNetworkRequest(QUrl(ipStr+tmpStr1+keyIndex)));
}

void lujiazui::scollTimerOut()
{
	if(pos>scollText.length())
       pos=0;
	ui.scolltextLabel->setText(scollText.mid(pos));
    pos++;
}

QString lujiazui::getCountDownDt(QDateTime qdtStart, QDateTime dtEnd)
{
	int subTimeSecs=dtEnd.toTime_t()-qdtStart.toTime_t();
	if(subTimeSecs>=0)
	{
		uint days=subTimeSecs/86400;
		uint subdays=subTimeSecs%86400;
		uint hours=subdays/3600;
		uint subhours=subdays%3600;
		uint mins=subhours/60;
		uint submins=subhours%60;
		uint secs=submins;
		if(isServicing)
			return QString("%3����").arg(mins);
		else
			return QString("%1��%2ʱ%3��").arg(days).arg(hours).arg(mins);
		
	}else{
		return "�Ѿ���ʱ";
	}
}


void lujiazui::finishiedServicer(QString serviceID)
{
	QString ipStr(usrAuth.serverip);
	QString curServiceID=ui.h_id_label->text();
	if(curServiceID==serviceID)
	{
		QString todayStr;
		QDateTime time = QDateTime::currentDateTime().addDays(0);//��ȡϵͳ���ڵ�ʱ��
		
		todayStr = time.toString("yyyy-MM-dd"); //������ʾ��ʽ
		QString md5Str=getMd5Check(todayStr+serviceID);
		QString tmpStr1(tr("/OAapp/WebObjects/OAapp.woa?cjApply=doctor_status&handle=over&id="));
		QString pathStr1=tr("&checkstr=");



		qDebug()<<ipStr+tmpStr1+serviceID+pathStr1+md5Str;
		checkState->get(QNetworkRequest(QUrl(ipStr+tmpStr1+serviceID+pathStr1+md5Str)));
	}
}

#if 0
void lujiazui::changeEvent(QEvent* event)
{  
	QWidget::changeEvent(event);
    if(event->type()==QEvent::WindowStateChange)
	{  
        if(windowState() & Qt::WindowMinimized)
		{  
			QTimer::singleShot(0, this, SLOT(hide()));
			event->ignore();  
			//createActions();
        }  
	}
}  
#endif


void lujiazui::createActions()
{
   trayIconMenu = new QMenu(this);
   action_quit=new QAction(this);

   //�����������ı�
   action_quit->setText(QString("�˳�"));
 
   //���Ӳ˵���
   trayIconMenu = new QMenu(this);
   trayIconMenu->addAction(action_quit);
   //action_setting->setVisible(false);
 
   trayIcon = new QSystemTrayIcon(this);
   trayIcon->setToolTip("ҽ����ϯ��");
   trayIcon->setIcon(QIcon("image/logox.ico"));                    // ��������ͼ
   trayIcon->setContextMenu(trayIconMenu);
   trayIcon->show();               // ������ͼ����ʾ����

   //connect(action_show, SIGNAL(triggered()), this, SLOT( showNormal()));
   connect(action_quit, SIGNAL(triggered()), this, SLOT(closeApp()));//http://blog.sina.com.cn/s/blog_5e42f31a0100xkt1.html�����
   //connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showWindows()));
   connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_trayIcon_activated(QSystemTrayIcon::ActivationReason)));

}


void lujiazui::showWindows()
{
    //timer->stop();
    //tray->setIcon(QIcon("image/icon.png"));   //������ʾʱ��ָ�ԭ��ͼ�꣬��ֹ��ʱ��������ʾͼ��ʱ����ֹ
	//setBaseSize( 800, 600 );
	//setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
	//setWindowFlags( Qt::Widget | Qt::WindowStaysOnTopHint );���������⣬һֱ�ڶ���top
   setWindowFlags( Qt::Widget  );
   this->showNormal();
}

void lujiazui::closeApp()
{
	exit(0);
}

void lujiazui::on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
     switch(reason)
     {
         case QSystemTrayIcon::Trigger:
         case QSystemTrayIcon::DoubleClick:
              setWindowFlags( Qt::Widget | Qt::WindowStaysOnTopHint );
              this->showNormal();
              break;
              break;
         case QSystemTrayIcon::MiddleClick:
              QMessageBox::information(this,"MiddleClick","trayIcon MiddleClicked!");
              break;
         default:
                 ;
     }
}
