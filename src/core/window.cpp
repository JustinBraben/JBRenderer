#include "stdafx.h"
#include "window.hpp"

Window::Window()
{
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::Init(int width, int height)
{
	glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_Window = glfwCreateWindow(width, height, "JBRenderer", NULL, NULL);
}