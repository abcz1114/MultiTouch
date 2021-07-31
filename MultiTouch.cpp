//
// Created by Matthew Good on 29/7/21.
//

#include "MultiTouch.h"

MultiTouch::TouchData::TouchData() :
    MultiTouch::TouchData(0, 0, 0, 0, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, float x, float y, MultiTouch::TouchState state) :
MultiTouch::TouchData(identity, x, y, 0, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, float x, float y, float size, MultiTouch::TouchState state) :
MultiTouch::TouchData(identity, x, y, size, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, float x, float y, float size, float pressure, MultiTouch::TouchState state) {
    this->identity = identity;
    this->x = x;
    this->y = y;
    this->size = size;
    this->pressure = pressure;
    this->state = state;
}

MultiTouch::TouchContainer::TouchContainer() {
    used = false;
}

MultiTouch::TouchContainer::TouchContainer(bool used, const TouchData & touch) {
    this->used = used;
    this->touch = touch;
}

MultiTouch::TouchData & MultiTouch::getTouchAt(long index) {
    return data[index].touch;
}

long MultiTouch::getTouchCount() {
    return touchCount;
}

long MultiTouch::getTouchIndex() {
    return index;
}

void MultiTouch::tryPurgeTouch(MultiTouch::TouchContainer & touchContainer) {
    if (!touchContainer.used && touchContainer.touch.state != NONE) {
        touchContainer.touch.moved = false;
        touchContainer.touch.state = NONE;
        touchCount--;
    }
}

void MultiTouch::addTouch(long identity, float x, float y, float size, float pressure) {
    if (debug) Log::Debug("adding touch with identity: ", identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && !touchContainer.used) {
            found = true;
            touchContainer.used = true;
            touchContainer.touch.identity = identity;
            touchContainer.touch.x = x;
            touchContainer.touch.y = y;
            touchContainer.touch.size = size;
            touchContainer.touch.pressure = pressure;
            touchContainer.touch.moved = false;
            touchContainer.touch.state = TOUCH_DOWN;
            touchCount++;
            index = i;
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "the maximum number of supported touches of ",
                maxSupportedTouches,
                " has been reached.\n",
                "please call setMaxSupportedTouches(long)"
        );
    }
}

void MultiTouch::addTouch(long identity, float x, float y, float size) {
    addTouch(identity, x, y, size, 0);
}

void MultiTouch::addTouch(long identity, float x, float y) {
    addTouch(identity, x, y, 0, 0);
}

void MultiTouch::moveTouch(long identity, float x, float y, float size, float pressure) {
    if (debug) Log::Debug("moving touch with identity: ", identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && touchContainer.used) {
            if (touchContainer.touch.identity == identity) {
                found = true;
                touchContainer.touch.moved = touchContainer.touch.x != x || touchContainer.touch.y != y;
                touchContainer.touch.x = x;
                touchContainer.touch.y = y;
                touchContainer.touch.size = size;
                touchContainer.touch.pressure = pressure;
                touchContainer.touch.state = TOUCH_MOVE;
                index = i;
            }
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "cannot move a touch that has not been registered"
        );
    }
}

void MultiTouch::moveTouch(long identity, float x, float y, float size) {
    moveTouch(identity, x, y, size, 0);
}

void MultiTouch::moveTouch(long identity, float x, float y) {
    moveTouch(identity, x, y, 0, 0);
}

void MultiTouch::removeTouch(long identity, float x, float y, float size, float pressure) {
    if (debug) Log::Debug("removing touch with identity: ", identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && touchContainer.used) {
            if (touchContainer.touch.identity == identity) {
                found = true;
                touchContainer.touch.moved = touchContainer.touch.x != x || touchContainer.touch.y != y;
                touchContainer.touch.x = x;
                touchContainer.touch.y = y;
                touchContainer.touch.size = size;
                touchContainer.touch.pressure = pressure;
                touchContainer.touch.state = TOUCH_UP;
                touchContainer.used = false;
                index = i;
            }
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "cannot remove a touch that has not been registered"
        );
    }
}

void MultiTouch::removeTouch(long identity, float x, float y, float size) {
    removeTouch(identity, x, y, size, 0);
}

void MultiTouch::removeTouch(long identity, float x, float y) {
    removeTouch(identity, x, y, 0, 0);
}

void MultiTouch::cancelTouch(long identity, float x, float y, float size, float pressure) {
    if (debug) Log::Debug("cancelling touch with identity: ", identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && touchContainer.used) {
            if (touchContainer.touch.identity == identity) {
                found = true;
                touchContainer.touch.moved = touchContainer.touch.x != x || touchContainer.touch.y != y;
                touchContainer.touch.x = x;
                touchContainer.touch.y = y;
                touchContainer.touch.size = size;
                touchContainer.touch.pressure = pressure;
                touchContainer.touch.state = TOUCH_CANCELLED;
                touchContainer.used = false;
                index = i;
            }
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "cannot remove a touch that has not been registered"
        );
    }
}

void MultiTouch::cancelTouch(long identity, float x, float y, float size) {
    cancelTouch(identity, x, y, size, 0);
}

void MultiTouch::cancelTouch(long identity, float x, float y) {
    cancelTouch(identity, x, y, 0, 0);
}

std::string MultiTouch::stateToString(const MultiTouch::TouchState & state) const {
    switch(state) {
        case NONE : return "NONE";
        case TOUCH_DOWN : return "TOUCH_DOWN";
        case TOUCH_MOVE : return "TOUCH_MOVE";
        case TOUCH_UP : return "TOUCH_UP";
        case TOUCH_CANCELLED : return "TOUCH_CANCELLED";
    }
}

std::string MultiTouch::toString() const {
    std::string s;
    s += "touch count : " + std::to_string(touchCount);
    for (long touchIndex = 0; touchIndex < maxSupportedTouches ; touchIndex++) {
        const TouchContainer & touchContainer = data[touchIndex];
        const TouchData & touch = touchContainer.touch;
        if (touch.state != NONE) {
            s += "\n touch index : " + std::to_string(touchIndex);
            if (touchIndex == index) s += " [CURRENT]";
            s += ", action : " + stateToString(touch.state);
            s += ", identity : " + std::to_string(touch.identity);
            s += ", did touch move : "; s += touch.moved ? "True" : "False";
            s += ", x : " + std::to_string(touch.x);
            s += ", y : " + std::to_string(touch.y);
            s += ", size : " + std::to_string(touch.size);
            s += ", pressure : " + std::to_string(touch.pressure);
        }
    }
    return s;
}
