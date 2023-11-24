#include "Timer.h"

CTimer::CTimer() {
	if (QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformance_Frequency)) {
		m_bHardware_Has_Performance_Counter = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nLast_Time);
		m_fTime_Scale = 1.0f / (double)m_nPerformance_Frequency;
	}
	else {
		m_bHardware_Has_Performance_Counter = FALSE;
		m_nLast_Time = timeGetTime();
		m_fTime_Scale = 0.001f;
	}

	m_nSample_Count = 0;
	m_nCurrent_FrameRate = 0;
	m_nFPS = 0;
	m_fTime_Cal_FPS = 0.0f;
}

CTimer::~CTimer() {
}

void CTimer::Reset() {
	__int64 nPerformance_Counter;
	QueryPerformanceCounter((LARGE_INTEGER*)&nPerformance_Counter);

	m_nLast_Time = nPerformance_Counter;
	m_nCurrent_Time = nPerformance_Counter;

	m_bStopped = false;
}

void CTimer::Tick(float fLock_FPS) {
	if (m_bStopped) {
		m_fElapsed_Time = 0.0f;
		return;
	}

	if (m_bHardware_Has_Performance_Counter) {
		QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrent_Time);
	}
	else {
		m_nCurrent_Time = timeGetTime();
	}

	float fElapsed_Time = float((m_nCurrent_Time - m_nLast_Time) * m_fTime_Scale);

	if (fLock_FPS > 0.0f) {
		while (fElapsed_Time < (1.0f / fLock_FPS)) {
			if (m_bHardware_Has_Performance_Counter) {
				QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrent_Time);
			}
			else {
				m_nCurrent_Time = timeGetTime();
			}

			fElapsed_Time = float((m_nCurrent_Time - m_nLast_Time) * m_fTime_Scale);
		}
	}

	m_nLast_Time = m_nCurrent_Time;

	if (fabsf(fElapsed_Time - m_fElapsed_Time) < 1.0f) {
		memmove(&m_fFrame_Time[1], m_fFrame_Time, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrame_Time[0] = fElapsed_Time;

		if (m_nSample_Count < MAX_SAMPLE_COUNT) {
			++m_nSample_Count;
		}
	}

	++m_nFPS;
	m_fTime_Cal_FPS += fElapsed_Time;

	if (m_fTime_Cal_FPS > 1.0f) {
		m_nCurrent_FrameRate = m_nFPS;
		m_nFPS = 0;
		m_fTime_Cal_FPS = 0.0f;
	}

	m_fElapsed_Time = 0.0f;
	for (ULONG i = 0; i < m_nSample_Count; ++i) {
		m_fElapsed_Time += m_fFrame_Time[i];
	}

	if (m_nSample_Count > 0) {
		m_fElapsed_Time /= m_nSample_Count;
	}
}

unsigned long CTimer::Get_FrameRate(LPTSTR lpszString, int nCharacters) {
	if (lpszString) {
		_itow_s(m_nCurrent_FrameRate, lpszString, nCharacters, 8);
		wcscat_s(lpszString, nCharacters, _T(" FPS)"));
	}

	return m_nCurrent_FrameRate;
}