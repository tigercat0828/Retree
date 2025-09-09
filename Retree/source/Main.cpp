#include "pch.h"
#include "Core/Window.h"
#include "Scenes/MainScene.h"
using namespace std;

int main(int argc, char* argv[]) {

	Window& app = Window::Instance();
	app.SetWindowTitle("Main Window");
	app.SetStartScene(make_unique<MainScene>());
	app.SetWindowSize(1080, 960);
	app.Run();
	return 0;
}




