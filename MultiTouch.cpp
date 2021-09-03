//
// Created by Matthew Good on 29/7/21.
//

#include "MultiTouch.h"

MultiTouch::TouchData::TouchData() :
    MultiTouch::TouchData(0, 0, 0, 0, 0, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, uint64_t timestamp, float x, float y, MultiTouch::TouchState state) :
MultiTouch::TouchData(identity, timestamp, x, y, 0, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, uint64_t timestamp, float x, float y, float size, MultiTouch::TouchState state) :
MultiTouch::TouchData(identity, timestamp, x, y, size, 0, MultiTouch::NONE)
{
}

MultiTouch::TouchData::TouchData(long identity, uint64_t timestamp, float x, float y, float size, float pressure, MultiTouch::TouchState state):
MultiTouch::TouchData(identity, timestamp, x, y, size, pressure, state, false)
{
}

MultiTouch::TouchData::TouchData(long identity, uint64_t timestamp, float x, float y, float size, float pressure, MultiTouch::TouchState state, bool moved) {
    this->identity = identity;
    this->timestamp = timestamp;
    this->x = x;
    this->y = y;
    this->size = size;
    this->pressure = pressure;
    this->state = state;
    this->moved = moved;
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

MultiTouch::Iterator::Iterator(MultiTouch * multiTouch) {
    this->multiTouch = multiTouch;
}

bool MultiTouch::Iterator::hasNext() {
    for (long i = index; i < multiTouch->maxSupportedTouches; i++) {
        TouchContainer & tc = multiTouch->data[i];
        // a container can be marked as unused but have a touch state != NONE
        // in this case, it is either freshly removed, or freshly cancelled
        if (tc.touch.state != NONE) {
            index = i;
            return true;
        }
    }
    return false;
}

MultiTouch::TouchData * MultiTouch::Iterator::next() {
    return &multiTouch->data[index++].touch;
}

long MultiTouch::Iterator::getIndex() const {
    return index-1;
}

MultiTouch::Iterator MultiTouch::getIterator() {
    return Iterator(this);
}

void MultiTouch::setMaxSupportedTouches(long supportedTouches) {
    maxSupportedTouches = supportedTouches;
    data.resize(maxSupportedTouches);
}

long MultiTouch::getMaxSupportedTouches() {
    return maxSupportedTouches;
}

void MultiTouch::tryPurgeTouch(MultiTouch::TouchContainer & touchContainer) {
    if (!touchContainer.used && touchContainer.touch.state != NONE) {
        touchContainer.touch.moved = false;
        touchContainer.touch.state = NONE;
        touchCount--;
    }
}

void MultiTouch::addTouch(const MultiTouch::TouchData & touchData) {
    if (debug) Log::Debug("adding touch with identity: ", touchData.identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && !touchContainer.used) {
            found = true;
            touchContainer.touch = touchData;
            touchContainer.touch.state = TOUCH_DOWN;
            touchContainer.used = true;
            touchCount++;
            index = i;
        }
        if (touchContainer.used && touchContainer.touch.state == NONE) {
            Log::Error_And_Throw("touch cannot be active with a state of NONE");
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

void MultiTouch::addTouch(long identity, float x, float y, float size, float pressure) {
    addTouch({identity, static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()), x, y, size, pressure, TOUCH_DOWN, false});
}

void MultiTouch::addTouch(long identity, float x, float y, float size) {
    addTouch(identity, x, y, size, 0);
}

void MultiTouch::addTouch(long identity, float x, float y) {
    addTouch(identity, x, y, 0, 0);
}

void MultiTouch::moveTouch(const MultiTouch::TouchData & touchData) {
    if (debug) Log::Debug("moving touch with identity: ", touchData.identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && touchContainer.used) {
            if (touchContainer.touch.identity == touchData.identity) {
                found = true;
                bool moved = touchContainer.touch.x != touchData.x || touchContainer.touch.y != touchData.y;
                touchContainer.touch = touchData;
                touchContainer.touch.moved = moved;
                touchContainer.touch.state = TOUCH_MOVE;
                index = i;
            }
        }
        if (touchContainer.used && touchContainer.touch.state == NONE) {
            Log::Error_And_Throw("touch cannot be active with a state of NONE");
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "cannot move a touch that has not been registered"
        );
    }
}

void MultiTouch::moveTouch(long identity, float x, float y, float size, float pressure) {
    moveTouch({identity, static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()), x, y, size, pressure, TOUCH_MOVE});
}

void MultiTouch::moveTouch(long identity, float x, float y, float size) {
    moveTouch(identity, x, y, size, 0);
}

void MultiTouch::moveTouch(long identity, float x, float y) {
    moveTouch(identity, x, y, 0, 0);
}

void MultiTouch::removeTouch(const MultiTouch::TouchData & touchData) {
    if (debug) Log::Debug("removing touch with identity: ", touchData.identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        tryPurgeTouch(touchContainer);
        if (!found && touchContainer.used) {
            if (touchContainer.touch.identity == touchData.identity) {
                found = true;
                bool moved = touchContainer.touch.x != touchData.x || touchContainer.touch.y != touchData.y;
                touchContainer.touch = touchData;
                touchContainer.touch.moved = moved;
                touchContainer.touch.state = TOUCH_UP;
                touchContainer.used = false;
                index = i;
            }
        }
        if (touchContainer.used && touchContainer.touch.state == NONE) {
            Log::Error_And_Throw("touch cannot be active with a state of NONE");
        }
    }
    if (!found) {
        Log::Error_And_Throw(
                "cannot remove a touch that has not been registered"
        );
    }
}

void MultiTouch::removeTouch(long identity, float x, float y, float size, float pressure) {
    removeTouch({identity, static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()), x, y, size, pressure, TOUCH_UP});
}

void MultiTouch::removeTouch(long identity, float x, float y, float size) {
    removeTouch(identity, x, y, size, 0);
}

void MultiTouch::removeTouch(long identity, float x, float y) {
    removeTouch(identity, x, y, 0, 0);
}

void MultiTouch::cancelTouch(const MultiTouch::TouchData & touchData) {
    if (debug) Log::Debug("cancelling touch with identity: ", touchData.identity);
    bool found = false;
    for (long i = 0; i < maxSupportedTouches; i++) {
        TouchContainer & touchContainer = data[i];
        if (touchContainer.used && touchContainer.touch.state == NONE) {
            Log::Error_And_Throw("touch cannot be active with a state of NONE");
        }
        // ignore touch identity since we are cancelling a touch
        // the identity may not match at all
        if (touchContainer.used) {
            if (!found) {
                found = true;
                bool moved = touchContainer.touch.x != touchData.x || touchContainer.touch.y != touchData.y;
                touchContainer.touch = touchData;
                touchContainer.touch.moved = moved;
                touchContainer.touch.state = TOUCH_CANCELLED;
                touchContainer.used = false;
                index = i;
            } else {
                touchContainer.touch.state = NONE;
                touchContainer.used = false;
            }
        }
    }
    
    if (!found) {
        // if not found, cancel the first touch
        TouchContainer & touchContainer = data[0];
        bool moved = touchContainer.touch.x != touchData.x || touchContainer.touch.y != touchData.y;
        touchContainer.touch = touchData;
        touchContainer.touch.moved = moved;
        touchContainer.touch.state = TOUCH_CANCELLED;
        touchContainer.used = false;
        index = 0;
    }
    touchCount = 0;
}

void MultiTouch::cancelTouch(long identity, float x, float y, float size, float pressure) {
    cancelTouch({identity, static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()), x, y, size, pressure, TOUCH_CANCELLED});
}

void MultiTouch::cancelTouch(long identity, float x, float y, float size) {
    cancelTouch(identity, x, y, size, 0);
}

void MultiTouch::cancelTouch(long identity, float x, float y) {
    cancelTouch(identity, x, y, 0, 0);
}

std::string MultiTouch::stateToString(const MultiTouch::TouchState & state) {
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
