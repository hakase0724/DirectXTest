#pragma once
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <list>

using namespace std;

//�����擾�Ɏg�p����API
#define FPSCOUNTER_QUERYPER_COUNTER        1
#define FPSCOUNTER_TIMEGETTIME             2
class FPSCountor
{
public:
	FPSCountor(unsigned int smp = 10);
	~FPSCountor();
	//FPS�l���擾
	double GetFPS();
	//�T���v������ύX
	void SetSampleNum(unsigned int smp);
	//���݂̎������擾
	double GetCurDefTime();
	//FPS���X�V
	double UpdateFPS(double Def);
private:
	//�g�p����v���֐��̔���t���O
	int mCounterFlg = 0;
	//�N���b�N�J�E���g��
	LARGE_INTEGER mCounter;
	//1�b������N���b�N�J�E���g���i���g���j
	double mFreq = 0.0;
	//�ȑO�̃N���b�N�J�E���g��
	LONGLONG mBeforeClockCount = 0;
	//�ȑO�̎����i�~���b�j
	DWORD mBeforeMillSecond = 0; 
	//���ԃ��X�g
	list<double> mTimeList;
	//�ړ����όv�Z���̃f�[�^��
	UINT mTimeDataCount = 0;     
	//���ʕ����̍��v�l
	double mSumTimes = 0.0;               
};

