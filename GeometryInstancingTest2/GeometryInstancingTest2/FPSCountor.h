#pragma once
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <list>

using namespace std;

//時刻取得に使用するAPI
#define FPSCOUNTER_QUERYPER_COUNTER        1
#define FPSCOUNTER_TIMEGETTIME             2
class FPSCountor
{
public:
	FPSCountor(unsigned int smp = 10);
	~FPSCountor();
	//FPS値を取得
	double GetFPS();
	//サンプル数を変更
	void SetSampleNum(unsigned int smp);
	//現在の時刻を取得
	double GetCurDefTime();
	//FPSを更新
	double UpdateFPS(double Def);
private:
	//使用する計測関数の判定フラグ
	int mCounterFlg = 0;
	//クロックカウント数
	LARGE_INTEGER mCounter;
	//1秒当たりクロックカウント数（周波数）
	double mFreq = 0.0;
	//以前のクロックカウント数
	LONGLONG mBeforeClockCount = 0;
	//以前の時刻（ミリ秒）
	DWORD mBeforeMillSecond = 0; 
	//時間リスト
	list<double> mTimeList;
	//移動平均計算時のデータ数
	UINT mTimeDataCount = 0;     
	//共通部分の合計値
	double mSumTimes = 0.0;               
};

