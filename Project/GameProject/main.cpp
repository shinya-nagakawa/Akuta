#include "Base/Base.h"
#include "Game/Field.h"
#include "Game/Player.h"
#include "Game/Weapon.h"
#include "Game/Item.h"
#include "Game/Camera.h"
#include "Game/Enemy.h"
#include "Game/Sellpoint.h"
#include "Game/BuyPoint.h"
#include "Game/Game.h"
#include "Game/Title.h"
#include "Game/BlackFilter.h"
#include "Game/DebugUI.h"
#include "Game/UI.h"
#include "Effekseer/EffekseerManager.h"
#include "Tutorial/Renderer.h"

//モーション有モデルクラス
CModelA3M model;
CImage img;

//必須（imguiフォルダのソースファイルすべて）
//「imgui」を検索し参照
//https://github.com/ocornut/imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_ja_gryph_ranges.h"

// グローバルで sunset を UI にリンク
DebugUI* debugUI = nullptr;
float sunset = 0.1;



void MainLoop(void) {
	//--------------------------------------------------------------
	//ゲーム中の動きはここに書く
	//ゲーム中はこの関数_を1秒間に60回呼び出している
	//--------------------------------------------------------------


	CRendaring::GetInstance()->Render([]() {
		Base::RenderALL();
		});
	Base::DrawALL();
	CInput::Update();
	Base::CheckKillALL();
	Base::UpdateALL();
	Base::CollisionALL();

	
	if (HOLD(CInput::eButton10)) {
		sunset = max(sunset - 0.001f, 0);
	}

	sunset = max(sunset - 0.000005f, 0);

	glClearColor(0.0 * sunset, 0.1 * sunset, 0.1 * sunset, 1.0);

	//ライト設定
	CLight::SetType(2, CLight::eLight_Direction);
	CLight::SetPos(2, CVector3D(0, 200, 200));
	CLight::SetDir(2, CVector3D(0.5, -1, 0.5).GetNormalize());
	CLight::SetColor(2, CVector3D(0.4f * sunset, 0.4f * sunset, 0.4f * sunset), CVector3D(0.6f * sunset, 0.6f * sunset, 0.6f * sunset));
	//CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));

}

void Init(void)
{
	//debugUI = new DebugUI(&sunset);
		
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClearColor(0.0 * sunset, 0.0 * sunset, 0.0 * sunset, 1.0);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);//ブレンドの有効化
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//固定シェーダー用
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_ALPHA_TEST);

	CFPS::SetFPS(0);
	//フレーム制御初期化
	CFPS::Init();
	//ボタンの設定
	CInput::Init();
	CInput::SetButton(0, CInput::eButton1, 'Z');
	CInput::SetButton(0, CInput::eButton2, 'E');
	CInput::SetButton(0, CInput::eButton3, 'F');
	CInput::SetButton(0, CInput::eButton4, VK_LSHIFT);
	CInput::SetButton(0, CInput::eButton5, VK_SPACE);
	CInput::SetButton(0, CInput::eButton6, 'R');
	CInput::SetButton(0, CInput::eButton7, 'Q');
	CInput::SetButton(0, CInput::eButton10, VK_RETURN);
	CInput::SetButton(0, CInput::eNum1, '1');
	CInput::SetButton(0, CInput::eNum2, '2');
	CInput::SetButton(0, CInput::eNum3, '3');
	CInput::SetButton(0, CInput::eNum4, '4');
	CInput::SetButton(0, CInput::eNum5, '5');
	CInput::SetButton(0, CInput::eUp, 'W');
	CInput::SetButton(0, CInput::eDown, 'S');
	CInput::SetButton(0, CInput::eLeft, 'A');
	CInput::SetButton(0, CInput::eRight, 'D');
	CInput::SetButton(0, CInput::eMouseL, VK_LBUTTON);
	CInput::SetButton(0, CInput::eMouseR, VK_RBUTTON);
	CInput::SetButton(0, CInput::eMouseC, VK_MBUTTON);

	//マウス非表示
	//CInput::ShowCursor(false);
	CInput::SetMouseInside(true);
	CInput::Update();
	CInput::Update();

	CLight::SetFogParam(CVector4D(1, 1, 1, 1), 1200, 1400);

	//カメラ初期化
	CCamera::GetCamera()->LookAt(CVector3D(5, 5, 5),
		CVector3D(0, 0, 0),
		CVector3D(0.0, 1.0, 0.0));

	SetCurrentDirectory("data");

	CLight::SetType(0, CLight::eLight_Spot);

	//ポストエフェクトを生成		画面解像度,被写界深度オフセット
	CRendaring::CreatInstance(SCREEN_WIDTH, SCREEN_HEIGHT, CVector3D(0.1f, 0.85f, 0.5f), CRendaring::RendringType::eForward);
	CRendaring::GetInstance()->SetEdge(false);
	CRendaring::GetInstance()->SetDOF(true);
	CShadow::CreateInscance(40, 20, 4096, 4096);
	CShadow::SetShadowAmbient(0.4);

	CShader::GetInstance("StaticMesh");
	CShader::GetInstance("SkinMesh");
	CSound::GetInstance();
	CLight::Init();



	//-----------------------------------------------------
	//初期化の命令を書く
	//ゲーム起動時に一度だけ呼ばれる
	//-----------------------------------------------------

	//imgui必須--------------------------------------------------------------------
// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//日本語入力用フォント指定
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 32.0f, NULL, glyphRangesJapanese);
	//(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(GL::window, false);
	ImGui_ImplOpenGL2_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//-------------------------------------------------------------------------

	ADD_RESOURCE("Golem", CModel::CreateModel("Charactor/Golem/Golem.a3m"));
	ADD_RESOURCE("Spider", CModel::CreateModel("Charactor/Spider/Spider.a3m"));
	ADD_RESOURCE("GreenDragon", CModel::CreateModel("Charactor/Doragon/GreenDoragon2.a3m"));
	ADD_RESOURCE("vanguard", CModel::CreateModel("Charactor/Vanguard/Vansin.a3m"));
	ADD_RESOURCE("Sword", CModel::CreateModel("Wepon/Sword/Sword.obj"));
	ADD_RESOURCE("Jet", CModel::CreateModel("Charactor/Vanguard/Vansin.a3m"));
	ADD_RESOURCE("Light", CModel::CreateModel("Effect/Light.obj"));
	ADD_RESOURCE("Gauge", CImage::CreateImage("UI/gauge2.png"));
	ADD_RESOURCE("RedFilter", CImage::CreateImage("UI/red_filter.png"));
	ADD_RESOURCE("GreenFilter", CImage::CreateImage("UI/green_filter.png"));
	ADD_RESOURCE("BlackFilter", CImage::CreateImage("UI/black_filter.png"));
	ADD_RESOURCE("Inventory", CImage::CreateImage("UI/inventory.png"));

	ADD_RESOURCE("GameClear", CImage::CreateImage("UI/GameClear.png"));

	SOUND("音楽")->Load("BGM/onngaku.wav", 1);
	SOUND("タイトル音楽")->Load("BGM/taitoruongaku.wav");
	SOUND("持ち上げる")->Load("BGM/put_heavy_box.wav");
	SOUND("ため攻撃")->Load("BGM/手足・締め上げる.wav",1);
	SOUND("剣落とす")->Load("BGM/sei_ge_fokusupun_otosu03.wav");
	SOUND("売却音")->Load("BGM/Cash_Register-Drawer02-1_Low_.wav");
	SOUND("売却中")->Load("BGM/baikyakutyu.wav");
	SOUND("足音")->Load("BGM/arukuoto (mono).wav");
	SOUND("声")->Load("BGM/Syaberukoe.wav");

	//Base::Add(new Game());
	Base::Add(new Title());
	//SOUND("タイトル音楽")->Volume(0.0);
	
	
	
}


void Release()
{
	Base::ClearInstance();
	CLoadThread::ClearInstance();
	CSound::ClearInstance();
	CResourceManager::ClearInstance();
	// ImGuiの終了処理
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

static void ResizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);

	//画面解像度変動
	CCamera::GetCamera()->SetSize((float)w, (float)h);
	//画面解像度固定
	//CCamera::GetCamera()->SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	CCamera::GetCamera()->Viewport(0, 0, (float)w, (float)h);
	CCamera::GetCurrent()->Perspective(DtoR(60.0), (float)w / (float)h, 0.1, 1000.0);

	glfwGetWindowPos(window, &GL::window_x, &GL::window_y);
	GL::UpdateWindowRect(GL::window_x, GL::window_y, w, h);
	CInput::UpdateClipCursor(true);

}
static void WheelCallback(GLFWwindow* _window, double _offsetx, double _offsety) {
	CInput::AddMouseWheel((int)_offsety);

}
static void PosCallback(GLFWwindow* _window, int x, int y) {
	GL::window_x = x;
	GL::window_y = y;
	GL::UpdateWindosRect(x, y, GL::window_width, GL::window_height);
	CInput::UpdateClipCursor(true);

}
static void FocusCallback(GLFWwindow* _window, int f) {
	CInput::UpdateClipCursor(f);
	GL::focus = f;
}

static void error_callback(int error, const char* description)
{
	printf("Error: %s\n", description);
}

//フルスクリーン?ウインドウモードの切り替え
//Alt+Enterで切り替える
void CheckFullScreen() {
	static int key_enter = 0;
	int key_enter_buf = key_enter;
	if (key_enter_buf ^ (key_enter = glfwGetKey(GL::window, GLFW_KEY_ENTER)) && key_enter && (glfwGetKey(GL::window, GLFW_KEY_LEFT_ALT) || glfwGetKey(GL::window, GLFW_KEY_RIGHT_ALT))) {
		GL::ChangeFullScreen(!GL::full_screen);
	}
}

int __main(int* argcp, char** argv) {
	// メモリリーク検出
	//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//　.dllのパス指定
	char path[256];
	GetCurrentDirectory(256, path);
	strcat_s(path, 255, "\\dll");
	SetDllDirectory(path);

	//OpenGL4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) return -1;
	//	glutInit(argcp, argv);

	GL::window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple", nullptr, nullptr);
	glfwGetWindowSize(GL::window, &GL::window_width, &GL::window_height);
	glfwGetWindowPos(GL::window, &GL::window_x, &GL::window_y);

	glfwSetFramebufferSizeCallback(GL::window, ResizeCallback);
	glfwSetScrollCallback(GL::window, WheelCallback);
	glfwSetWindowFocusCallback(GL::window, FocusCallback);
	glfwSetWindowPosCallback(GL::window, PosCallback);
	if (!GL::window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(GL::window);
	glfwSwapInterval(0);
	ResizeCallback(GL::window, SCREEN_WIDTH, SCREEN_HEIGHT);

	GLenum err = glewInit();
	if (err == GLEW_OK) {
		printf("%s\n", glewGetString(GLEW_VERSION));
	}
	else {
		printf("%s\n", glewGetErrorString(err));
		getchar();
		return -1;
	}
	HDC glDc = wglGetCurrentDC();
	GL::hWnd = WindowFromDC(glDc);
	SetWindowLong(GL::hWnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(GL::hWnd, HWND_TOP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_SHOWWINDOW);

	Init();


	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER time_buf;
	QueryPerformanceCounter(&time_buf);
	float fps = 0;
	while (!glfwWindowShouldClose(GL::window)) {
		CheckFullScreen();
		//各バッファーをクリア
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MainLoop();
		//FONT_T()->Draw(0, 32, 1, 0, 0, "fps:%3d", (int)CFPS::GetFPS(), (int)fps);



		glfwSwapBuffers(GL::window);
		CFPS::Wait();

		glfwPollEvents();
		if (glfwGetKey(GL::window, GLFW_KEY_ESCAPE)) {
			GL::isQuit = true;
			glfwSetWindowShouldClose(GL::window, GL_TRUE);
		}

	}

	glfwTerminate();

	Release();
	return 0;
}

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR argv, INT argc)
{
	return __main(&__argc, __argv);
}

int main(int argc, char** argv)
{
	return __main(&argc, argv);
}