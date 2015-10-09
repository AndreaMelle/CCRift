#if !defined(__NANOGUI_SCREEN_H)
#define __NANOGUI_SCREEN_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class Screen : public Widget
{
    friend class Widget;
    friend class Window;
public:
	Screen(GLFWwindow* w);
    virtual ~Screen();

	/*
		Override this to populate the screen with your widgets
	*/
	virtual void create(int winWidth, int winHeight) = 0;
	virtual void didResize(int winWidth, int winHeight) = 0;

	bool onKey(GLFWwindow* w, int key, int scancode, int action, int mods);
	void onResize(GLFWwindow* w, int width, int height);
	void onCursorPos(GLFWwindow* w, double x, double y);
	bool onMouseButton(GLFWwindow* w, int button, int action, int modifiers);
	void onScroll(GLFWwindow* w, double x, double y);

    void render();

    void setVisible(bool visible);
    void setSize(const Vector2i& size);
	void setDraggable(bool drag) { mDrawAllowed = drag; }

    virtual bool keyboardEvent(int key, int scancode, bool press, int modifiers);
    inline Vector2i mousePos() const { return mMousePos; }
    NVGcontext *nvgContext() { return mNVGContext; }

protected:
    void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);

protected:
    NVGcontext *mNVGContext;
    std::vector<Widget *> mFocusPath;
    Vector2i mFBSize;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
	bool mDrawAllowed;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_SCREEN_H */
