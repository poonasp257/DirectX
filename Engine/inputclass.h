////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <map>
#include <utility>

#include <dinput.h>

enum class KeyCode {
	W, A, S, D,
	Escape, F1
};

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();
	
	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	
	bool GetKey(KeyCode key);
	bool GetKeyDown(KeyCode key);
	bool GetKeyUp(KeyCode key);

	void GetMouseLocation(int&, int&);
	void GetMouseDeltaPosition(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();
private:
	IDirectInput8*						m_directInput;
	IDirectInputDevice8*				m_keyboard;
	IDirectInputDevice8*				m_mouse;
	DIMOUSESTATE						m_mouseState;
	std::map<KeyCode, unsigned char>	m_keyState;
	unsigned char						m_keyboardState[256];
	unsigned char						m_pressedKeys[256];
	int									m_screenWidth, m_screenHeight;
	int									m_mouseX, m_mouseY;

};



#endif