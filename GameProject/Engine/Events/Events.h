#pragma once

// TO SUBSCRIBE TO THE EVENTBUS
// EventBus::get().subscribe(this, &ClassName::functionName);
// FUNCTION NEEDS TO HAVE THIS STRUCTURE
// functionName(EventType * evnt)

// TO PUBLISH TO THE EVENTBUS
// EventBus::get().publish(new EventType(param1, param2));

// THE SYNTAX FOR UNSUBSCRIBE IS THE SAME AS FOR SUBSCRIBE

class Event
{
};

struct KeyEvent : public Event
{
	KeyEvent(int key, int action) : key{ key }, action{ action } {};
	int key;
	int action;
};

struct WindowResizeEvent : public Event
{
	WindowResizeEvent(int width, int height) : width{ width }, height{ height } {};
	int width;
	int height;
};

struct MouseMoveEvent : public Event
{
	MouseMoveEvent(double moveX, double moveY) : moveX{ moveX }, moveY{ moveY } {};
	double moveX;
	double moveY;
};

struct MouseClickEvent : public Event
{
	MouseClickEvent(int button, int action) : button{ button }, action{ action } {};
	int button;
	int action;
};