#include "common.h"
#include <string>
#include <QString>
#include <QtCore/QSharedMemory>  

extern SipInfo  usrAuth; //用户信息
extern LinphoneCore *the_core;

bool ReadUserConfig()
{

   QSettings *configIniRead = new QSettings("userinfo.ini", QSettings::IniFormat);  
   //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型   
   QString usrStr = configIniRead->value("/sipuser/usr").toString();  
   strcpy(usrAuth.usrname,usrStr.toStdString().c_str());
  
   QString pwdStr = configIniRead->value("/sipuser/pwd").toString(); 
   strcpy(usrAuth.password,pwdStr.toStdString().c_str());
   
   QString sipIPStr = configIniRead->value("/sipuser/sipIP").toString(); 
   strcpy(usrAuth.sipip,sipIPStr.toStdString().c_str());

   QString serverIPStr = configIniRead->value("/sipuser/serverIP").toString(); 
   strcpy(usrAuth.serverip,serverIPStr.toStdString().c_str());

   //打印得到的结果   
   qDebug()<<"sipIPStr" << usrAuth.usrname;  
   qDebug() <<"serverIPStr"<< serverIPStr;  
   //读入入完成后删除指针   
   delete configIniRead;  

   return true;
}


bool RegisterSip()
{
	char identity[128];
	ReadUserConfig();
	
	//先退出当前注册
	linphone_core_clear_proxy_config(the_core);
	if((strlen(usrAuth.usrname)==0)||(strlen(usrAuth.password)==0)||(strlen(usrAuth.sipip)==0))
		return false;

	sprintf(identity,"sip:%s@%s",usrAuth.usrname,usrAuth.sipip);

	//检查是否已经注册，如果没有注册则注册
	LinphoneProxyConfig* proxyCfg=NULL;	

	//LinphoneProxyConfig* proxyCfg=new LinphoneProxyConfig;	
	linphone_core_get_default_proxy(the_core,&proxyCfg);
	if (proxyCfg!=NULL)
	{
		if (linphone_proxy_config_is_registered(proxyCfg))
		{
			return false;
		}else
		{
			linphone_core_clear_proxy_config(the_core);
		}
		
	}
	//应该是写配置信息
	proxyCfg = linphone_proxy_config_new();

	// add username password
	LinphoneAddress *from = linphone_address_new(identity);
	LinphoneAuthInfo *info;
	if (from !=0){
		info=linphone_auth_info_new(linphone_address_get_username(from),NULL,usrAuth.password,NULL,NULL);
		linphone_core_add_auth_info(the_core,info);
	}
	linphone_address_destroy(from);

	// configure proxy entries
	linphone_proxy_config_set_identity(proxyCfg,identity);
	linphone_proxy_config_set_server_addr(proxyCfg,usrAuth.sipip);
	linphone_proxy_config_enable_register(proxyCfg,true);
	linphone_proxy_config_set_dial_escape_plus(proxyCfg,TRUE);
	linphone_core_add_proxy_config(the_core,proxyCfg);

	//set to default proxy
	linphone_core_set_default_proxy(the_core,proxyCfg);
	return true;
}

QString getMd5Check( QString md5key )
{
	QString md5Value;
    QByteArray bb;  
    bb = QCryptographicHash::hash(md5key.toAscii(),QCryptographicHash::Md5);  
    md5Value.append(bb.toHex());  
    return  md5Value.toUpper();
}

int assumeSingleInstance(const char* program)  
{  
    static QSharedMemory shm(program);  
    if(shm.create(100) == false)  
    {  
        return -1;  
    }  
    return 0;  
}  