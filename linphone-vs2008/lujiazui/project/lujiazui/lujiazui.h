#ifndef lujiazui_H
#define lujiazui_H

#include <QtGui/QDialog>
#include "ui_lujiazui.h"
#include "common.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class lujiazui : public QDialog
{
	Q_OBJECT
public:
	lujiazui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~lujiazui();

	//���������˽�еģ������ò��Ƶ�������
	Ui::lujiazuiClass ui;
	QString keyIndex;
	int oldItems,newItems;
	QString scollText;
    int pos;
	QString countDownDt;
	QDateTime servStartDt;
	QString getCountDownDt(QDateTime qdtStart,QDateTime qdtEnd);
	bool isServicing;
	int countMins;
	QTimer *scollTimer;
	QTimer *updateTimer;
	void finishiedServicer(QString serviceID);
	QString currentCallTel;
	QVector<DoctoryInfo>  docInfoVector;//Ĭ����������QHash
    
	//void changeEvent(QEvent* event);
	QNetworkAccessManager  *pushClientRequest;
	int callNums;
	int sendNums;
    bool iscalled;
public  slots:
	void linponeIterate();
	//�����Ҷϰ�ť
	void on_sipcallBtn_clicked();
	void on_siphungupBtn_clicked();
	//�б�ˢ�°�ť
	void on_listUpdateBtn_clicked();
	//������״̬�޸İ�ť
	void on_serviceCsBtn_clicked();
	//��ȡԤԼ�б�
	//void replyDocClassFinished(QNetworkReply *reply);	
	//��ȡԤԼ�б�
	void replyDocFinished(QNetworkReply *reply);
	//�޸ķ�����״̬
	void replyCSFinished(QNetworkReply *reply);
	void changeCurrent(const QModelIndex &current,const QModelIndex &previous);
	void comboBoxValueChanged(const QString currentText);
	void sendIFFinished(QNetworkReply *reply);
	void pushClientFinished(QNetworkReply *reply);
	void updateAppoitment();
	void scollTimerOut();
	//�˳���¼
	void on_signoutBtn_clicked();

	void showWindows();

	void on_trayIcon_activated(QSystemTrayIcon::ActivationReason reason);
	void closeApp();

	//void openUrl(const QString);

private:
	//��������б�
	QNetworkAccessManager  *sendInfoRequest;

	//��������б�
	QNetworkAccessManager  *managerDoctoryClass;
	//�����б�
	QNetworkAccessManager  *managerDoctory;
	//�ӷ��������޸�״̬
	QNetworkAccessManager  *checkState;
	QStandardItemModel* initModel;
	QMap<QString,DoctoryInfo>  map_docinfo;


	QMap<QString,DoctoryClassList> map_docclass;
	QMap<QString,QString> sendInfoMap;
	void loadClassJson();
	void loadDocJson();
	void initClassList();
	void initDocTable();
	QString getLoginId();
	int compareTime();

	QAction  *action_quit;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

	void createTrayIcon();
	void createActions();
};

#endif // lujiazui_H
