#include "ExampleApplication.h"

void ExampleApp::_start_w_timer()
{
	SetTimer(mWindow, W_TIMER, 400, NULL);
}
void ExampleApp::_stop_w_timer()
{
	KillTimer(mWindow, W_TIMER);
}

void ExampleApp::_start_s_timer()
{
	SetTimer(mWindow, S_TIMER, 400, NULL);

}
void ExampleApp::_stop_s_timer()
{
	KillTimer(mWindow, S_TIMER);

}

void ExampleApp::_start_a_timer()
{
	SetTimer(mWindow, A_TIMER, 400, NULL);

}
void ExampleApp::_stop_a_timer()
{
	KillTimer(mWindow, A_TIMER);

}

void ExampleApp::_start_d_timer()
{
	SetTimer(mWindow, D_TIMER, 400, NULL);

}
void ExampleApp::_stop_d_timer()
{
	KillTimer(mWindow, D_TIMER);

}


