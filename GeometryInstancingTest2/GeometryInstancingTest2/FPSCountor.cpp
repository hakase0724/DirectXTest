#include "stdafx.h"
#include "FPSCountor.h"


FPSCountor::FPSCountor(unsigned int smp)
{
	//�T���v�����̐ݒ�
	SetSampleNum(smp);
	//�v�����鎞�v�̑I��
	if (QueryPerformanceCounter(&mCounter) != 0)
	{
		//QueryPerformanceCounter�֐����g���t���O
		mCounterFlg = FPSCOUNTER_QUERYPER_COUNTER;
		//�������̎����i�N���b�N���j���擾
		mBeforeClockCount = mCounter.QuadPart;        
		LARGE_INTEGER Freq;
		//1�b������N���b�N�����擾
		QueryPerformanceFrequency(&Freq);            
		mFreq = (double)Freq.QuadPart;
	}
	else
	{
		//timeGetTime�֐����g���t���O
		mCounterFlg = FPSCOUNTER_TIMEGETTIME;
		//���x���グ��
		timeBeginPeriod(1);
		//�������̎����i�~���b�j���擾
		mBeforeMillSecond = timeGetTime();
	}
}


FPSCountor::~FPSCountor()
{
	//�^�C�}�[�̐��x��߂�
	if (mCounterFlg == FPSCOUNTER_TIMEGETTIME)
		timeEndPeriod(1);    
}

double FPSCountor::GetFPS()
{
	double Def = GetCurDefTime();
	//�v�Z�ł��Ȃ��̂ł�Ԃ�
	if (Def == 0) return 0;
	return UpdateFPS(Def);
}

void FPSCountor::SetSampleNum(unsigned int smp)
{
	//���ς��v�Z�����
	mTimeDataCount = smp; 
	//���X�g������
	mTimeList.resize(mTimeDataCount, 0.0);    
	mSumTimes = 0;
}

double FPSCountor::GetCurDefTime()
{
	//�������Ԃ��v��
	if (mCounterFlg == FPSCOUNTER_QUERYPER_COUNTER)
	{
		//QueryPerformanceCounter�֐��ɂ��v��
		QueryPerformanceCounter(&mCounter);
		LONGLONG LongDef = mCounter.QuadPart - mBeforeClockCount;
		double dDef = (double)LongDef;
		mBeforeClockCount = mCounter.QuadPart;
		return dDef * 1000 / mFreq;
	}
	//timeGetTime�֐��ɂ��v��
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - mBeforeMillSecond;
	mBeforeMillSecond = CurTime;
	return CurDef;
}

double FPSCountor::UpdateFPS(double Def)
{
	//�ł��Â��f�[�^������
	mTimeList.pop_front();
	//�V�����f�[�^��ǉ�
	mTimeList.push_back(Def);
	//FPS�Z�o
	double FPS;
	double AveDef = (mSumTimes + Def) / mTimeDataCount;
	if (AveDef != 0) FPS = 1000.0 / AveDef;
	//���ʉ��Z�����̍X�V
	mSumTimes += Def - *mTimeList.begin();
	return FPS;
}
