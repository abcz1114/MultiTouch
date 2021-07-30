# MultiTouch
a cross platform MultiTouch library that aims to provide a unified MultiTouch API

REQUIRES linking to DiligentCommon - https://github.com/mgood7123/DiligentCore

# USAGE

### Mac OS

to add MultiTouch support to your application, adapt your source to the following

inside `NSView` that subclasses a `NSResponder`, for example, `NSOpenGLView`

implementation (.mm)
```ObjectiveC
- (void)updateTrackingAreas {
    [self initTrackingArea];
}

-(void) initTrackingArea {
    NSTrackingAreaOptions options = (
        NSTrackingActiveAlways |
        NSTrackingInVisibleRect |
        NSTrackingMouseMoved
     );

    NSTrackingArea *area = [
        [NSTrackingArea alloc]
        initWithRect:[self bounds]
        options:options
        owner:self
        userInfo:nil
    ];
    [self addTrackingArea:area];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

// https://developer.apple.com/library/archive/documentation/Cocoa/Conceptual/EventOverview/HandlingTouchEvents/HandlingTouchEvents.html#//apple_ref/doc/uid/10000060i-CH13-SW21

- (void)touchesBeganWithEvent:(NSEvent *)event {
    [_renderer touchesBeganWithEvent:[event touchesMatchingPhase:NSTouchPhaseBegan inView:self]];
}

- (void)touchesMovedWithEvent:(NSEvent *)event {
    [_renderer touchesMovedWithEvent:[event touchesMatchingPhase:NSTouchPhaseMoved inView:self]];
}

- (void)touchesEndedWithEvent:(NSEvent *)event {
    [_renderer touchesEndedWithEvent:[event touchesMatchingPhase:NSTouchPhaseEnded inView:self]];
}

- (void)touchesCancelledWithEvent:(NSEvent *)event {
    [_renderer touchesCancelledWithEvent:[event touchesMatchingPhase:NSTouchPhaseAny inView:self]];
}

- (void) awakeFromNib

{
    [self setAcceptsTouchEvents:YES];
    // ...
}
```

inside a callback class, eg `_renderer`

interface (.h)
```ObjectiveC
// include path/to/MultiTouch.h here

@interface OpenGLRenderer : NSObject
{
    MultiTouch multiTouch;

    // something that consumes MultiTouch objects
    AppInstance appInstance;
}

- (instancetype) init;
- (void) touchesBeganWithEvent:(NSSet *) touches;
- (void) touchesMovedWithEvent:(NSSet *) touches;
- (void) touchesEndedWithEvent:(NSSet *) touches;
- (void) touchesCancelledWithEvent:(NSSet *) touches;

@end
```
implementation (.mm)
```ObjectiveC
@implementation OpenGLRenderer

- (instancetype) init {
    multiTouch.setMaxSupportedTouches(10);
    return self;
}

- (void)touchesBeganWithEvent:(NSSet *) touches {
    NSArray *array = [touches allObjects];
    NSUInteger numberOfTouches = [array count];
    for (unsigned long i = 0; i < numberOfTouches; i++) {
        NSTouch *t = [array objectAtIndex:i];
        multiTouch.addTouch(
            ((NSInteger) t.identity),
            t.normalizedPosition.x,
            t.normalizedPosition.y
        );
        appInstance.onTouchEvent(multiTouch);
    }
}

- (void)touchesMovedWithEvent:(NSSet *) touches {
    NSArray *array = [touches allObjects];
    NSUInteger numberOfTouches = [array count];
    for (unsigned long i = 0; i < numberOfTouches; i++) {
        NSTouch *t = [array objectAtIndex:i];
        multiTouch.moveTouch(
            ((NSInteger) t.identity),
            t.normalizedPosition.x,
            t.normalizedPosition.y
        );
        appInstance.onTouchEvent(multiTouch);
    }
}

- (void)touchesEndedWithEvent:(NSSet *) touches {
    NSArray *array = [touches allObjects];
    NSUInteger numberOfTouches = [array count];
    for (unsigned long i = 0; i < numberOfTouches; i++) {
        NSTouch *t = [array objectAtIndex:i];
        multiTouch.removeTouch(
            ((NSInteger) t.identity),
            t.normalizedPosition.x,
            t.normalizedPosition.y
        );
        appInstance.onTouchEvent(multiTouch);
    }
}

- (void)touchesCancelledWithEvent:(NSSet *) touches {
    NSArray *array = [touches allObjects];
    NSUInteger numberOfTouches = [array count];
    for (unsigned long i = 0; i < numberOfTouches; i++) {
        NSTouch *t = [array objectAtIndex:i];
        multiTouch.cancelTouch(
            ((NSInteger) t.identity),
            t.normalizedPosition.x,
            t.normalizedPosition.y
        );
        appInstance.onTouchEvent(multiTouch);
    }
}
```

### Android

to add MultiTouch support to android, adapt your views OnTouchEvent to the following

```Java
@Override
public boolean onTouchEvent(MotionEvent event) {
    int action = event.getActionMasked();
    int actionIndex = event.getActionIndex();
    int id = 0;
    boolean isUpDown = false;
    switch (action) {
        case MotionEvent.ACTION_DOWN:
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_POINTER_DOWN:
        case MotionEvent.ACTION_POINTER_UP:
            id = event.getPointerId(actionIndex);
            isUpDown = true;
            break;
    }

    int c = event.getPointerCount();
    for (int i = 0; i < c; i++) {
        int pid = event.getPointerId(i);
        if (isUpDown) {
            if (actionIndex == i) {
                switch (action) {
                    case MotionEvent.ACTION_DOWN:
                    case MotionEvent.ACTION_POINTER_DOWN:
                        renderer.addTouch(
                                id,
                                event.getX(actionIndex),
                                event.getY(actionIndex),
                                event.getSize(actionIndex),
                                event.getPressure(actionIndex)
                        );
                        break;
                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_POINTER_UP:
                        renderer.removeTouch(
                                id,
                                event.getX(actionIndex),
                                event.getY(actionIndex),
                                event.getSize(actionIndex),
                                event.getPressure(actionIndex)
                        );
                        break;
                }
            }
        }

        if (actionIndex != i || !isUpDown) {
            renderer.moveTouch(
                    pid,
                    event.getX(i),
                    event.getY(i),
                    event.getSize(i),
                    event.getPressure(i)
            );
        }
    }
    return renderer.onTouchEvent(renderer.nativeInstance);
}
```

and add the following native calls

```Java
native void addTouch(long identity, float x, float y);
native void addTouch(long identity, float x, float y, float size);
native void addTouch(long identity, float x, float y, float size, float pressure);
native void moveTouch(long identity, float x, float y);
native void moveTouch(long identity, float x, float y, float size);
native void moveTouch(long identity, float x, float y, float size, float pressure);
native void removeTouch(long identity, float x, float y);
native void removeTouch(long identity, float x, float y, float size);
native void removeTouch(long identity, float x, float y, float size, float pressure);
native void cancelTouch(long identity, float x, float y);
native void cancelTouch(long identity, float x, float y, float size);
native void cancelTouch(long identity, float x, float y, float size, float pressure);
native boolean onTouchEvent(long instance);
```

finally, add the native implementation

```C++
#include <MultiTouch.h>
MultiTouch multiTouch;

#define APP(jlong) reinterpret_cast<YourNativeClassInstance*>(jlong)

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_addTouch__JFF(
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y
)
{
    multiTouch.addTouch(identifier, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_addTouch__JFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size
)
{
    multiTouch.addTouch(identifier, x, y, size);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_addTouch__JFFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size, jfloat pressure
)
{
    multiTouch.addTouch(identifier, x, y, size, pressure);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_moveTouch__JFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y
)
{
    multiTouch.moveTouch(identifier, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_moveTouch__JFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size
)
{
    multiTouch.moveTouch(identifier, x, y, size);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_moveTouch__JFFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size, jfloat pressure
)
{
    multiTouch.moveTouch(identifier, x, y, size, pressure);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_removeTouch__JFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y
)
{
    multiTouch.removeTouch(identifier, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_removeTouch__JFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size
)
{
    multiTouch.removeTouch(identifier, x, y, size);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_removeTouch__JFFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size, jfloat pressure
)
{
    multiTouch.removeTouch(identifier, x, y, size, pressure);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_cancelTouch__JFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y
)
{
    multiTouch.cancelTouch(identifier, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_cancelTouch__JFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size
)
{
    multiTouch.cancelTouch(identifier, x, y, size);
}

extern "C" JNIEXPORT void JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_cancelTouch__JFFFF (
        JNIEnv * env, jobject thiz,
        jlong identifier, jfloat x, jfloat y, jfloat size, jfloat pressure
)
{
    multiTouch.cancelTouch(identifier, x, y, size, pressure);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_smallville7123_graphical_tool_kit_DiligentEngineView_00024DiligentEngineRenderer_onTouchEvent (
        JNIEnv * env, jobject thiz, jlong instance
)
{
    return APP(instance)->onTouchEvent(multiTouch);
}
```