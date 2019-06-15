#include "stdafx.h"
#include "FPSCountor.h"


FPSCountor::FPSCountor(unsigned int smp)
{
	//サンプル数の設定
	SetSampleNum(smp);
	//計測する時計の選択
	if (QueryPerformanceCounter(&mCounter) != 0)
	{
		//QueryPerformanceCounter関数を使うフラグ
		mCounterFlg = FPSCOUNTER_QUERYPER_COUNTER;
		//生成時の時刻（クロック数）を取得
		mBeforeClockCount = mCounter.QuadPart;        
		LARGE_INTEGER Freq;
		//1秒当たりクロック数を取得
		QueryPerformanceFrequency(&Freq);            
		mFreq = (double)Freq.QuadPart;
	}
	else
	{
		//timeGetTime関数を使うフラグ
		mCounterFlg = FPSCOUNTER_TIMEGETTIME;
		//精度を上げる
		timeBeginPeriod(1);
		//生成時の時刻（ミリ秒）を取得
		mBeforeMillSecond = timeGetTime();
	}
}


FPSCountor::~FPSCountor()
{
	//タイマーの精度を戻す
	if (mCounterFlg == FPSCOUNTER_TIMEGETTIME)
		timeEndPeriod(1);    
}

double FPSCountor::GetFPS()
{
	double Def = GetCurDefTime();
	//計算できないのでを返す
	if (Def == 0) return 0;
	return UpdateFPS(Def);
}

void FPSCountor::SetSampleNum(unsigned int smp)
{
	//平均を計算する個数
	mTimeDataCount = smp; 
	//リスト初期化
	mTimeList.resize(mTimeDataCount, 0.0);    
	mSumTimes = 0;
}

double FPSCountor::GetCurDefTime()
{
	//差分時間を計測
	if (mCounterFlg == FPSCOUNTER_QUERYPER_COUNTER)
	{
		//QueryPerformanceCounter関数による計測
		QueryPerformanceCounter(&mCounter);
		LONGLONG LongDef = mCounter.QuadPart - mBeforeClockCount;
		double dDef = (double)LongDef;
		mBeforeClockCount = mCounter.QuadPart;
		return dDef * 1000 / mFreq;
	}
	//timeGetTime関数による計測
	DWORD CurTime = timeGetTime();
	DWORD CurDef = CurTime - mBeforeMillSecond;
	mBeforeMillSecond = CurTime;
	return CurDef;
}

double FPSCountor::UpdateFPS(double Def)
{
	//最も古いデータを消去
	mTimeList.pop_front();
	//新しいデータを追加
	mTimeList.push_back(Def);
	//FPS算出
	double FPS;
	double AveDef = (mSumTimes + Def) / mTimeDataCount;
	if (AveDef != 0) FPS = 1000.0 / AveDef;
	//共通加算部分の更新
	mSumTimes += Def - *mTimeList.begin();
	return FPS;
}
