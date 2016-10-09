/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include "Leap.h"
using namespace Leap;
#include	<stdio.h>
#include	<stdlib.h>
#include	<windows.h>
#include	<tchar.h>

//通信ポート番号の設定。PCの環境に合わせて、最後の数値を書き換える
#define	TARGET_PORT	_T("\\\\.\\COM3")
//送受信バッファのサイズ
#define	BUFF_SZ	(64)

HANDLE hCom;

int sendmessage(HANDLE hCom, char *wbuf, char *rbuf);
HANDLE make_handle();

class SampleListener : public Listener {
private:
	char rbuf[BUFF_SZ];
	char wbuf[BUFF_SZ];
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);
};

void SampleListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void SampleListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void SampleListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  if (!frame.hands().isEmpty()) {
    // Get the first hand
    const Hand hand = frame.hands()[0];

    // Get the hand's sphere radius and palm position
//	std::cout << hand.palmPosition()[0] << std::endl;
	sprintf_s(wbuf, "%d", (int)hand.palmPosition()[0]);
	sendmessage(hCom, wbuf, rbuf);
	sendmessage(hCom, "e", rbuf);
	//std::cout << "wbuf:";
	std::cout << wbuf << std::endl;
	//std::cout << "rbuf:";
	//std::cout << rbuf << std::endl;
  }
}

void SampleListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();
}

void SampleListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}

////////////////////////////////////////////////////////////////
//	・VS-RC003/HVによるシリアル通信プログラムサンプル	'SerialSample.cpp'
//		(c)Vstone
//
//		本ソースはMicrosoft Visual C++（VC6.0以降）用です。
//		それ以外の開発環境をご利用の場合は、通信処理部分等をお使いの環境に応じて書き換えてください。
//
//		また、'TARGET_PORT'で定義されたシリアルポート番号を、お使いのPCの環境に応じて書き換えてください。
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//	main関数
////////////////////////////////////////////////////////////////

int main()
{
	hCom = make_handle();
	char rbuf[BUFF_SZ], wbuf[BUFF_SZ];
	// Create a sample listener and controller
	SampleListener listener;
	Controller controller;

	// Have the sample listener receive events from the controller
	controller.addListener(listener);

	//通信ハンドルの取得・設定に失敗していたらプログラムを終了する
	if (hCom == INVALID_HANDLE_VALUE) return 0;

	//VS-RC003の制御をコントローラからシリアル通信に切り替える
	printf("VS-RC003をシリアル通信より制御します。\n\n");
	sprintf_s(wbuf, "e");

	//メッセージの送受信。通信が成功しなかったらプログラムを終了
	if (sendmessage(hCom, wbuf, rbuf)){
		CloseHandle(hCom);
		return 0;
	}

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	//取得したハンドルを閉じる
	CloseHandle(hCom);

	// Remove the sample listener when done
	controller.removeListener(listener);

	return 0;
}

////////////////////////////////////////////////////////////////
//	sendmessage関数
//	メッセージを送受信する関数。
//	
//	・引数
//		HANDLE hCom	通信に使用するハンドル
//		char *wbuf	送信メッセージバッファへのポインタ
//		char *rbuf	受信メッセージバッファへのポインタ
//
//	・戻り値
//		0	メッセージの送受信に成功
//		1	メッセージの送信に失敗
//		2	メッセージの受信に失敗
//		3	メッセージの送受信に失敗
////////////////////////////////////////////////////////////////

int sendmessage(HANDLE hCom, char *wbuf, char *rbuf)
{

	BOOL werr = FALSE, rerr = FALSE;
	DWORD wn = 0, rn = 1;
	int i = 0, cr_num = 0;

	//メッセージの送信
	if (!WriteFile(hCom, wbuf, (int)strlen(wbuf), &wn, NULL)) werr = TRUE;

	return (werr << 1) | rerr;

}


////////////////////////////////////////////////////////////////
//	make_handle関数
//	通信ハンドルを取得する関数。
//	
//	・戻り値
//		INVALID_HANDLE_VALUE	通信ハンドルの取得・設定に失敗
//		INVALID_HANDLE_VALUE以外	取得したハンドル
////////////////////////////////////////////////////////////////

HANDLE make_handle()
{
	HANDLE hCom = INVALID_HANDLE_VALUE;
	DCB    stDcb;
	COMMTIMEOUTS cto;

	//シリアルポートのハンドルを取得
	hCom = CreateFile(TARGET_PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

	//シリアルポートのハンドルの取得に失敗したら戻る
	if (hCom == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;


	//シリアルポートの通信設定の作成
	//VS-RC003/HVの場合、ボーレートは115200bps、バイトサイズは8、パリティビット無し、ストップビットは1となる
	memset(&stDcb, 0, sizeof(DCB));

	stDcb.BaudRate = CBR_115200;
	stDcb.fParity = 1;
	stDcb.ByteSize = 8;
	stDcb.Parity = NOPARITY;
	stDcb.StopBits = ONESTOPBIT;
	stDcb.EofChar = 26;

	//シリアルポートの通信設定の更新
	if (SetCommState(hCom, &stDcb) == FALSE){
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	//タイムアウトの設定の作成
	memset(&cto, 0, sizeof(COMMTIMEOUTS));
	cto.ReadTotalTimeoutConstant = 100;
	cto.WriteTotalTimeoutConstant = 200;

	//タイムアウトの設定の更新
	if (SetCommTimeouts(hCom, &cto) == FALSE) {
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	return hCom;
}