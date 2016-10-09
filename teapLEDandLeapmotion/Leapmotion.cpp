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

//�ʐM�|�[�g�ԍ��̐ݒ�BPC�̊��ɍ��킹�āA�Ō�̐��l������������
#define	TARGET_PORT	_T("\\\\.\\COM3")
//����M�o�b�t�@�̃T�C�Y
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
//	�EVS-RC003/HV�ɂ��V���A���ʐM�v���O�����T���v��	'SerialSample.cpp'
//		(c)Vstone
//
//		�{�\�[�X��Microsoft Visual C++�iVC6.0�ȍ~�j�p�ł��B
//		����ȊO�̊J�����������p�̏ꍇ�́A�ʐM���������������g���̊��ɉ����ď��������Ă��������B
//
//		�܂��A'TARGET_PORT'�Œ�`���ꂽ�V���A���|�[�g�ԍ����A���g����PC�̊��ɉ����ď��������Ă��������B
//
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//	main�֐�
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

	//�ʐM�n���h���̎擾�E�ݒ�Ɏ��s���Ă�����v���O�������I������
	if (hCom == INVALID_HANDLE_VALUE) return 0;

	//VS-RC003�̐�����R���g���[������V���A���ʐM�ɐ؂�ւ���
	printf("VS-RC003���V���A���ʐM��萧�䂵�܂��B\n\n");
	sprintf_s(wbuf, "e");

	//���b�Z�[�W�̑���M�B�ʐM���������Ȃ�������v���O�������I��
	if (sendmessage(hCom, wbuf, rbuf)){
		CloseHandle(hCom);
		return 0;
	}

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	//�擾�����n���h�������
	CloseHandle(hCom);

	// Remove the sample listener when done
	controller.removeListener(listener);

	return 0;
}

////////////////////////////////////////////////////////////////
//	sendmessage�֐�
//	���b�Z�[�W�𑗎�M����֐��B
//	
//	�E����
//		HANDLE hCom	�ʐM�Ɏg�p����n���h��
//		char *wbuf	���M���b�Z�[�W�o�b�t�@�ւ̃|�C���^
//		char *rbuf	��M���b�Z�[�W�o�b�t�@�ւ̃|�C���^
//
//	�E�߂�l
//		0	���b�Z�[�W�̑���M�ɐ���
//		1	���b�Z�[�W�̑��M�Ɏ��s
//		2	���b�Z�[�W�̎�M�Ɏ��s
//		3	���b�Z�[�W�̑���M�Ɏ��s
////////////////////////////////////////////////////////////////

int sendmessage(HANDLE hCom, char *wbuf, char *rbuf)
{

	BOOL werr = FALSE, rerr = FALSE;
	DWORD wn = 0, rn = 1;
	int i = 0, cr_num = 0;

	//���b�Z�[�W�̑��M
	if (!WriteFile(hCom, wbuf, (int)strlen(wbuf), &wn, NULL)) werr = TRUE;

	return (werr << 1) | rerr;

}


////////////////////////////////////////////////////////////////
//	make_handle�֐�
//	�ʐM�n���h�����擾����֐��B
//	
//	�E�߂�l
//		INVALID_HANDLE_VALUE	�ʐM�n���h���̎擾�E�ݒ�Ɏ��s
//		INVALID_HANDLE_VALUE�ȊO	�擾�����n���h��
////////////////////////////////////////////////////////////////

HANDLE make_handle()
{
	HANDLE hCom = INVALID_HANDLE_VALUE;
	DCB    stDcb;
	COMMTIMEOUTS cto;

	//�V���A���|�[�g�̃n���h�����擾
	hCom = CreateFile(TARGET_PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

	//�V���A���|�[�g�̃n���h���̎擾�Ɏ��s������߂�
	if (hCom == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;


	//�V���A���|�[�g�̒ʐM�ݒ�̍쐬
	//VS-RC003/HV�̏ꍇ�A�{�[���[�g��115200bps�A�o�C�g�T�C�Y��8�A�p���e�B�r�b�g�����A�X�g�b�v�r�b�g��1�ƂȂ�
	memset(&stDcb, 0, sizeof(DCB));

	stDcb.BaudRate = CBR_115200;
	stDcb.fParity = 1;
	stDcb.ByteSize = 8;
	stDcb.Parity = NOPARITY;
	stDcb.StopBits = ONESTOPBIT;
	stDcb.EofChar = 26;

	//�V���A���|�[�g�̒ʐM�ݒ�̍X�V
	if (SetCommState(hCom, &stDcb) == FALSE){
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	//�^�C���A�E�g�̐ݒ�̍쐬
	memset(&cto, 0, sizeof(COMMTIMEOUTS));
	cto.ReadTotalTimeoutConstant = 100;
	cto.WriteTotalTimeoutConstant = 200;

	//�^�C���A�E�g�̐ݒ�̍X�V
	if (SetCommTimeouts(hCom, &cto) == FALSE) {
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	return hCom;
}