#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <iostream>
#include <map>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #if defined(WINAPI_PARTITION_DESKTOP)
        #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
            #define NANOVG_MSW
        #else
            #error "Compile error: WINRT not supported"
        #endif
    #else
        #define NANOVG_MSW
    #endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #error "Compile error: Linux not supported"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #if TARGET_OS_IPHONE
        #error "Compile error: iOS not supported"
    #else
        #define NANOVG_MAC
    #endif
#else
    #error "Nanovg compile error: Unknown platform"
#endif

#ifdef NANOVG_MAC
    #define NANOVG_GL2_IMPLEMENTATION
#else
    #ifdef NANOVG_MSW
        #define NANOVG_GL3_IMPLEMENTATION
    #endif
#endif

#include "nanovg_gl.h"

#ifdef NANOVG_GL3_IMPLEMENTATION
    #define NANOVG_CREATE nvgCreateGL3
    #define NANOVG_DELETE nvgDeleteGL3
#else
    #ifdef NANOVG_GL2_IMPLEMENTATION
        #define NANOVG_CREATE nvgCreateGL2
        #define NANOVG_DELETE nvgDeleteGL2
    #endif
#endif

NANOGUI_NAMESPACE_BEGIN

Screen::Screen(GLFWwindow* w)
	: Widget(nullptr)
	, mDrawAllowed(true)
{

#ifdef NDEBUG
    mNVGContext = NANOVG_CREATE(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#else
    mNVGContext = NANOVG_CREATE(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG);
#endif

    mTheme = new Theme(mNVGContext);
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = glfwGetTime();
    mProcessEvents = true;
    mVisible = false;

	glfwMakeContextCurrent(w);
	glfwGetFramebufferSize(w, &mFBSize[0], &mFBSize[1]);
	glfwGetWindowSize(w, &mSize[0], &mSize[1]);
	glViewport(0, 0, mFBSize[0], mFBSize[1]);

	

}

Screen::~Screen() {
    delete mTheme;
    NANOVG_DELETE(mNVGContext);
}

bool Screen::onKey(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	if (!mProcessEvents) return false;

	mLastInteraction = glfwGetTime();

	try
	{
		return keyboardEvent(key, scancode, action == GLFW_PRESS, mods);
	}
	catch (const std::exception &e)
	{
		//std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
		//abort();
	}

	return false;
}
void Screen::onResize(GLFWwindow* w, int width, int height)
{
	glfwMakeContextCurrent(w);
	glfwGetFramebufferSize(w, &mFBSize[0], &mFBSize[1]);
	glfwGetWindowSize(w, &mSize[0], &mSize[1]);
	glViewport(0, 0, mFBSize[0], mFBSize[1]);

	didResize(mSize[0], mSize[1]);
}

void Screen::onCursorPos(GLFWwindow* w, double x, double y)
{
	if (!mProcessEvents) return;
	
	Eigen::Vector2i p((int)x, (int)y);
	bool ret = false;
	mLastInteraction = glfwGetTime();
	
	try
	{
		if (mDragActive)
			ret = mDragWidget->mouseDragEvent(p - mDragWidget->parent()->absolutePosition(),
			p - mMousePos,
			mMouseState, mModifiers);
		else
			mouseMotionEvent(p,
			p - mMousePos,
			mMouseState,
			mModifiers);

		mMousePos = p;
	}
	catch (const std::exception &e) {
		//std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
		//abort();
	}
}

bool Screen::onMouseButton(GLFWwindow* w, int button, int action, int modifiers)
{
	if (!mProcessEvents) return false;

	mModifiers = modifiers;
	mLastInteraction = glfwGetTime();
	try {
		if (mFocusPath.size() > 1) {
			const Window *window = dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
			if (window && window->modal()) {
				if (!window->contains(mMousePos))
					return false;
			}
		}

		if (action == GLFW_PRESS)
			mMouseState |= 1 << button;
		else
			mMouseState &= ~(1 << button);

		if (mDrawAllowed)
		{
			if (mDragActive && action == GLFW_RELEASE &&
				findWidget(mMousePos) != mDragWidget)
				mDragWidget->mouseButtonEvent(
				mMousePos - mDragWidget->parent()->absolutePosition(),
				button, false, mModifiers);

			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
				mDragWidget = findWidget(mMousePos);
				if (mDragWidget == this)
					mDragWidget = nullptr;
				mDragActive = mDragWidget != nullptr;
				if (!mDragActive)
					updateFocus(nullptr);
			}
			else {
				mDragActive = false;
				mDragWidget = nullptr;
			}
		}
		

		return mouseButtonEvent(mMousePos, button, action == GLFW_PRESS, mModifiers);
	}
	catch (const std::exception &e) {
		//std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
		//abort();
	}

	return false;
}

void Screen::onScroll(GLFWwindow* w, double x, double y)
{
	if (!mProcessEvents) return;
	mLastInteraction = glfwGetTime();
	try
	{
		if (mFocusPath.size() > 1) {
			const Window *window = dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
			if (window && window->modal()) {
				if (!window->contains(mMousePos))
					return;
			}
		}
		scrollEvent(mMousePos, Vector2f(x, y));
	}
	catch (const std::exception &e) {
		//std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
		//abort();
	}
}

void Screen::setVisible(bool visible) {
    if (mVisible != visible) {
        mVisible = visible;
    }
}

void Screen::setSize(const Vector2i& size) {
    Widget::setSize(size);
}

void Screen::render()
{
    /* Calculate pixel ratio for hi-dpi devices. */
    mPixelRatio = (float)mFBSize[0] / (float)mSize[0];
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

    nvgTranslate(mNVGContext, -2, -2);
	Screen::draw(mNVGContext);

    double elapsed = glfwGetTime() - mLastInteraction;

    if (elapsed > 0.5f) {
        /* Draw tooltips */
        const Widget *widget = findWidget(mMousePos);
        if (widget && !widget->tooltip().empty()) {
            int tooltipWidth = 150;

            float bounds[4];
            nvgFontFace(mNVGContext, "sans");
            nvgFontSize(mNVGContext, 15.0f);
            nvgTextAlign(mNVGContext, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgTextLineHeight(mNVGContext, 1.1f);
            Vector2i pos =
                widget->absolutePosition() +
                Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
                             widget->tooltip().c_str(), nullptr, bounds);

            nvgGlobalAlpha(mNVGContext,
                           std::min(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(mNVGContext);
            nvgFillColor(mNVGContext, Color(0, 255));
            int h = (bounds[2] - bounds[0]) / 2;
            nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
                           (int)(bounds[2] - bounds[0]) + 8,
                           (int)(bounds[3] - bounds[1]) + 8, 3);

            int px = (int)((bounds[2]+bounds[0])/2) - h;
            nvgMoveTo(mNVGContext, px,bounds[1] - 10);
            nvgLineTo(mNVGContext, px+7,bounds[1]+1);
            nvgLineTo(mNVGContext, px-7,bounds[1]+1);
            nvgFill(mNVGContext);

            nvgFillColor(mNVGContext, Color(255, 255));
            nvgFontBlur(mNVGContext, 0.0f);
            nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(mNVGContext);

}

bool Screen::keyboardEvent(int key, int scancode, bool press, int modifiers) {
    if (Widget::keyboardEvent(key, scancode, press, modifiers))
        return true;

    return false;
}

void Screen::updateFocus(Widget *widget) {
    for (auto w: mFocusPath) {
        if (!w->focused())
            continue;
        w->focusEvent(false);
    }
    mFocusPath.clear();
    Widget *window = nullptr;
    while (widget) {
        mFocusPath.push_back(widget);
        if (dynamic_cast<Window *>(widget))
            window = widget;
        widget = widget->parent();
    }
    for (auto it = mFocusPath.rbegin(); it != mFocusPath.rend(); ++it)
        (*it)->focusEvent(true);

    if (window)
        moveWindowToFront((Window *) window);
}

void Screen::disposeWindow(Window *window) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
    if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
        mFocusPath.clear();
    if (mDragWidget == window)
        mDragWidget = nullptr;
    delete window;
}

void Screen::centerWindow(Window *window) {
    if (window->size() == Vector2i::Zero()) {
        window->setSize(window->preferredSize(mNVGContext));
        window->performLayout(mNVGContext);
    }
    window->setPosition((mSize - window->size()) / 2);
}

void Screen::moveWindowToFront(Window *window) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
    mChildren.push_back(window);
    /* Brute force topological sort (no problem for a few windows..) */
    bool changed = false;
    do {
        size_t baseIndex = 0;
        for (size_t index = 0; index < mChildren.size(); ++index)
            if (mChildren[index] == window)
                baseIndex = index;
        changed = false;
        for (size_t index = 0; index < mChildren.size(); ++index) {
            Popup *pw = dynamic_cast<Popup *>(mChildren[index]);
            if (pw && pw->parentWindow() == window && index < baseIndex) {
                moveWindowToFront(pw);
                changed = true;
                break;
            }
        }
    } while (changed);
}

NANOGUI_NAMESPACE_END
