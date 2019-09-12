//
// An abstract base class for state classes.
// Provides two public member functions to be used by classes that listen to the state classes.
// Virtual destructor, which in all of the inherited classes, does absolutely nothing;
//      it is there merely to make this class an abstract class. Nothing special.
//
// TO DO: Make this class abstract or non-instantiatable.
//

#ifndef JSWEEP_STATE_H
#define JSWEEP_STATE_H

#include "common.h"

#include <forward_list>
#include <functional>

template<typename T> class State {
protected:
    std::forward_list<std::function<void(const T &)>> observerList;
    // Pure virtual destr to make this class an abstract class.
    
public:
    /**
     * Notifies all observers listening to this class.
     */
    void NotifyAll() {
        for (auto observer : observerList)
            observer(*(static_cast<T*>(this)));
    }

    /**
     * Registers a function that will be called whenever NotifyAll is called.
     * Usually called by listeners.
     * @param fcn Function that will be called when NotifyAll is called.
     */
    void Register(std::function<void(const T &)> fcn) {
        observerList.push_front(fcn);
    }
};

#endif //JSWEEP_STATE_H
