#pragma once
#include "core/window.hpp"
#include "renderer/renderer.hpp"

class App
{
public:
	int Run();

protected:
	void Init();
    void Update();
    void Shutdown();

protected:
    // RendererDevice m_pDevice;
	Window m_Window;
    Renderer m_Renderer;

private:
	void Init_Internal();
	void Update_Internal();
	void Shutdown_Internal();
	void OnWindowResizeOrMove(int width, int height);
};