#pragma once

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const std::string& title, bool resizable = true);
    ~Window();

    GLFWwindow* GetHandle() const { return m_Window; }
    bool ShouldClose() const;
    void PollEvents() const;
    
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
    std::string m_Title;
};