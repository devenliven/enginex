#ifndef UTILITIES_TIMER_H_
#define UTILITIES_TIMER_H_

#include <profileapi.h>

class Timer
{
  public:
    Timer()
    {
        QueryPerformanceFrequency(&m_frequency);
        QueryPerformanceCounter(&m_startTime);
        m_lastTime = m_startTime;
    }

    float getTime() const
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        return static_cast<float>(currentTime.QuadPart - m_startTime.QuadPart) / static_cast<float>(m_frequency.QuadPart);
    }

    float getDeltaTime()
    {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        float deltaTime = static_cast<float>(currentTime.QuadPart - m_lastTime.QuadPart) / static_cast<float>(m_frequency.QuadPart);

        m_lastTime = currentTime;
        return deltaTime;
    }

    void reset()
    {
        QueryPerformanceCounter(&m_startTime);
        m_lastTime = m_startTime;
    }

  private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTime;
};

#endif // UTILITIES_TIMER_H_