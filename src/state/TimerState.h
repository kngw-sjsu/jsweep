//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_TIMERSTATE_H
#define JSWEEP_TIMERSTATE_H

#include <wx/timer.h>
#include "State.h"

class TimerState : public State<TimerState> {
    wxTimer timer;
    wxInt32 timeWhenStopped; // -1: Initial, 0: Ready, >= 1: Running
public:
    TimerState();
    ~TimerState(){PRINT_MSG("TimerState destr called");}

    /**
     * Checks whether the timer is running or not.
     * @return Flag indicating the timer is currently running.
     */
    bool IsRunning() const {
        return timer.IsRunning();
    }

    /**
     * Starts the timer with the interval of 1 second.
     */
    void Start() {
        timer.Start(1000);
    }

    /**
     * Stops the timer.
     * @param time Time (in seconds) taken since the timer started.
     */
    void Stop(wxInt32 time) {
        timeWhenStopped = time;
        timer.Stop();
    }

    /**
     * Sets the window to attach the timer to.
     * @param window wxEvtHandler-derived window class to attach the timer to.
     */
    void SetWindowToBeTimed(wxEvtHandler* window) {
        timer.SetOwner(window);
    }
};

#endif //JSWEEP_TIMERSTATE_H
