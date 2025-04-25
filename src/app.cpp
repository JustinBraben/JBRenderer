#include "stdafx.h"
#include "app.hpp"

int App::Run()
{
	Init_Internal();
	while (!glfwWindowShouldClose(m_Window.GetNativeWindow()))
	{
		Update_Internal();
	}
	Shutdown_Internal();
	return 0;
}

void App::Init()
{
    m_Renderer.Init(m_Window.GetNativeWindow());
}

void App::Update()
{

}

void App::Shutdown()
{

}

void App::Init_Internal()
{
    m_Window.Init(1024, 1024);

    // m_pDevice = new RendererDevice();

    Init();
}

void App::Update_Internal()
{
    glfwPollEvents();
}

void App::Shutdown_Internal()
{

}

void App::OnWindowResizeOrMove(int width, int height)
{

}