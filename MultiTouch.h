//
// Created by Matthew Good on 29/7/21.
//

#ifndef GRAPHICAL_TOOL_KIT_MOTIONEVENT_MULTITOUCH_H
#define GRAPHICAL_TOOL_KIT_MOTIONEVENT_MULTITOUCH_H

#include <Array.h>
#include <string>

class MultiTouch {
public:
    enum TouchState {
        NONE,
        TOUCH_DOWN,
        TOUCH_MOVE,
        TOUCH_UP,
        TOUCH_CANCELLED
    };

    class TouchData {
    public:
        long identity;
        float x;
        float y;
        float size;
        float pressure;
        bool moved;
        TouchState state;
        TouchData();
        TouchData(long identity, float x, float y, TouchState state);
        TouchData(long identity, float x, float y, float size, TouchState state);
        TouchData(long identity, float x, float y, float size, float pressure, TouchState state);
    };
    
private:
    class TouchContainer {
    public:
        bool used;
        TouchData touch;
        TouchContainer();
        TouchContainer(bool used, const TouchData & touch);
    };
    
    Array<TouchContainer> data;
    
    long maxSupportedTouches = 0;
    long touchCount = 0;
    long index;

public:
    
    TouchData & getTouchAt(long index);
    long getTouchCount();
    long getTouchIndex();

    void setMaxSupportedTouches(long supportedTouches) {
        maxSupportedTouches = supportedTouches;
        data.resize(maxSupportedTouches);
    }
    
    void tryPurgeTouch(MultiTouch::TouchContainer & touchContainer);

    void addTouch(long identity, float x, float y);
    void addTouch(long identity, float x, float y, float size);
    void addTouch(long identity, float x, float y, float size, float pressure);

    void moveTouch(long identity, float x, float y);
    void moveTouch(long identity, float x, float y, float size);
    void moveTouch(long identity, float x, float y, float size, float pressure);

    void removeTouch(long identity, float x, float y);
    void removeTouch(long identity, float x, float y, float size);
    void removeTouch(long identity, float x, float y, float size, float pressure);

    void cancelTouch(long identity, float x, float y);
    void cancelTouch(long identity, float x, float y, float size);
    void cancelTouch(long identity, float x, float y, float size, float pressure);

    std::string stateToString(const MultiTouch::TouchState & state) const;
    
    std::string toString() const;
};

#endif //GRAPHICAL_TOOL_KIT_MOTIONEVENT_MULTITOUCH_H
