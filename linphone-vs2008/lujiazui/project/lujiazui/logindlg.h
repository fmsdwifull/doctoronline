#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include "ui_logindlg.h"
#include "common.h"

class loginDlg : public QDialog
{
	Q_OBJECT

public:
	loginDlg(QDialog *parent = 0);
	~loginDlg();

private:
	Ui::loginDlg ui;
	QNetworkAccessManager  *loginRequest;


public slots:
	void on_loginBtn_clicked();
	void replyLoginFinished(QNetworkReply *reply);
};

#endif // LOGINDLG_H
