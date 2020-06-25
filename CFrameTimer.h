#pragma once

#include "stdafx.h"



class CFrameTimer
{
public:
    // 멤버변수와 생성/소멸자. 
    CFrameTimer() 
    {
        next_game_tick = timeGetTime();
    }

    virtual ~CFrameTimer() {}

protected:
    const int  TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;

    DWORD next_game_tick;
    bool _bFrameSkip = false;
    int iFrame = 0;


public:
    bool Update()
    {
        static DWORD _AccumulTime = 0;
        static DWORD _AccumulExeedTime = 0;
        int _Balance = 0;
        DWORD currentTitme = timeGetTime();

        {
            static int iCount = 0;
            static DWORD dwTick = 0;
                iCount++;
            if (dwTick + 1000 < timeGetTime())
            {
                wprintf(L"myTCP Figter Server : %d, FPS : %d\n", iCount, iFrame);
                iCount = 0;
                dwTick = timeGetTime();
                iFrame = 0;
            }
        }


        _AccumulTime = (currentTitme - next_game_tick);
        _Balance = SKIP_TICKS - _AccumulTime;
        // 40ms 보다 빠르게 while문이 돈 경우
        if (_Balance > 0)
        {
            //Sleep(_Balance);
            _bFrameSkip = false;
        }
        // 40ms보다 느리게 while문이 돈 경우
        else 
        {
            if (_Balance > -40)
                _bFrameSkip = true;
            else
            {
                _bFrameSkip = true;
            }
            iFrame++;

        }

        next_game_tick = currentTitme + _Balance;

        return true;

    }

    bool IsFrameSkip()
    {
        return _bFrameSkip;
    }

    void ResetFrameSkip()
    {
        _bFrameSkip = false;
    }

};
