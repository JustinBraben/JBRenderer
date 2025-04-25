#pragma once

struct GLFWwindow;

class Window
{
public:
	Window();
	~Window();

	void Init(int width, int height);

	// static Vector2i GetDisplaySize();

	bool PollMessages();

	void SetTitle(const char* pTitle);

	GLFWwindow* GetNativeWindow() const { return m_Window; }
	// Vector2i GetRect() const { return Vector2i(m_DisplayWidth, m_DisplayHeight); }

private:

private:
    GLFWwindow* m_Window = nullptr;
	bool m_Minimized = false;
	bool m_Maximized = false;
	int m_DisplayWidth = 0;
	int m_DisplayHeight = 0;
	bool m_IsResizing = false;
};