#ifndef CXWINDOWSSCREEN_H
#define CXWINDOWSSCREEN_H

#include "BasicTypes.h"
#include "ClipboardTypes.h"
#include "CMutex.h"
#include <X11/Xlib.h>

class IClipboard;
class CXWindowsClipboard;

class CXWindowsScreen {
public:
	CXWindowsScreen();
	virtual ~CXWindowsScreen();

protected:
	class CDisplayLock {
	public:
		CDisplayLock(const CXWindowsScreen*);
		~CDisplayLock();

		operator Display*() const;

	private:
		const CMutex*	m_mutex;
		Display*		m_display;
	};
	friend class CDisplayLock;

	// open the X display.  calls onOpenDisplay() after opening the display,
	// getting the screen, its size, and root window.  then it starts the
	// event thread.
	void				openDisplay();

	// destroy the window and close the display.  calls onCloseDisplay()
	// after the event thread has been shut down but before the display
	// is closed.
	void				closeDisplay();

	// get the opened screen, its size, its root window.  to get the
	// display create a CDisplayLock object passing this.  while the
	// object exists no other threads may access the display.  do not
	// save the Display* beyond the lifetime of the CDisplayLock.
	int					getScreen() const;
	void				getScreenSize(SInt32* w, SInt32* h) const;
	Window				getRoot() const;

	// create a cursor that is transparent everywhere
	Cursor				createBlankCursor() const;

	// wait for and get the next X event.  cancellable.
	bool				getEvent(XEvent*) const;

	// cause getEvent() to return false immediately and forever after
	void				doStop();

	// set the contents of the clipboard (i.e. primary selection)
	bool				setDisplayClipboard(ClipboardID,
								const IClipboard* clipboard);

	// copy the clipboard contents to clipboard
	bool				getDisplayClipboard(ClipboardID,
								IClipboard* clipboard) const;

	// called by openDisplay() to allow subclasses to prepare the display
	virtual void		onOpenDisplay() = 0;

	// called by openDisplay() after onOpenDisplay() to create each clipboard
	virtual CXWindowsClipboard*
						createClipboard(ClipboardID) = 0;

	// called by closeDisplay() to 
	virtual void		onCloseDisplay() = 0;

	// called when a clipboard is lost
	virtual void		onLostClipboard(ClipboardID) = 0;

private:
	// internal event processing
	bool				processEvent(XEvent*);

	// determine the clipboard from the X selection.  returns
	// kClipboardEnd if no such clipboard.
	ClipboardID			getClipboardID(Atom selection) const;

	// continue processing a selection request
	void				processClipboardRequest(Window window,
								Time time, Atom property);

	// terminate a selection request
	void				destroyClipboardRequest(Window window);

private:
	Display*			m_display;
	int					m_screen;
	Window				m_root;
	SInt32				m_w, m_h;
	bool				m_stop;

	// clipboards
	CXWindowsClipboard*	m_clipboard[kClipboardEnd];

	// X is not thread safe
	CMutex				m_mutex;
};

#endif
