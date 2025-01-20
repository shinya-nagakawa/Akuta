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

//���[�V�����L���f���N���X
CModelA3M model;
CImage img;

//�K�{�iimgui�t�H���_�̃\�[�X�t�@�C�����ׂāj
//�uimgui�v���������Q��
//https://github.com/ocornut/imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_ja_gryph_ranges.h"

// �O���[�o���� sunset �� UI �Ƀ����N
DebugUI* debugUI = nullptr;
//float sunset = 0.1;



void MainLoop(void) {
	//--------------------------------------------------------------
	//�Q�[�����̓����͂����ɏ���
	//�Q�[�����͂��̊֐�_��1�b�Ԃ�60��Ăяo���Ă���
	//--------------------------------------------------------------


	CRendaring::GetInstance()->Render([]() {
		Base::RenderALL();
		});
	Base::DrawALL();
	CInput::Update();
	Base::CheckKillALL();
	Base::UpdateALL();
	Base::CollisionALL();

	/*
	if (HOLD(CInput::eButton10)) {
		sunset = max(sunset - 0.001f, 0);
	}

	sunset = max(sunset - 0.000005f, 0);

	glClearColor(0.0 * sunset, 0.1 * sunset, 0.1 * sunset, 1.0);

	//���C�g�ݒ�
	CLight::SetType(2, CLight::eLight_Direction);
	CLight::SetPos(2, CVector3D(0, 200, 200));
	CLight::SetDir(2, CVector3D(0.5, -1, 0.5).GetNormalize());
	CLight::SetColor(2, CVector3D(0.4f * sunset, 0.4f * sunset, 0.4f * sunset), CVector3D(0.6f * sunset, 0.6f * sunset, 0.6f * sunset));
	//CLight::SetColor(1, CVector3D(0.4f, 0.4f, 0.4f), CVector3D(0.6f, 0.6f, 0.6f));*/

}

void Init(void)
{
	//debugUI = new DebugUI(&sunset);
		
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	//glClearColor(0.0 * sunset, 0.0 * sunset, 0.0 * sunset, 1.0);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);//�u�����h�̗L����
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//�Œ�V�F�[�_�[�p
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_ALPHA_TEST);

	CFPS::SetFPS(DEFAULT_FPS);
	//�t���[�����䏉����
	CFPS::Init();
	//�{�^���̐ݒ�
	CInput::Init();
	CInput::SetButton(0, CInput::eButton1, 'Z');
	CInput::SetButton(0, CInput::eButton2, 'E');
	CInput::SetButton(0, CInput::eButton3, 'F');
	CInput::SetButton(0, CInput::eButton4, VK_LSHIFT);
	CInput::SetButton(0, CInput::eButton5, VK_SPACE);
	CInput::SetButton(0, CInput::eButton6, 'R');
	CInput::SetButton(0, CInput::eButton7, 'Q');
	CInput::SetButton(0, CInput::eButton10, VK_RETURN);
	CInput::SetButton(0, CInput::eUp, 'W');
	CInput::SetButton(0, CInput::eDown, 'S');
	CInput::SetButton(0, CInput::eLeft, 'A');
	CInput::SetButton(0, CInput::eRight, 'D');
	CInput::SetButton(0, CInput::eMouseL, VK_LBUTTON);
	CInput::SetButton(0, CInput::eMouseR, VK_RBUTTON);
	CInput::SetButton(0, CInput::eMouseC, VK_MBUTTON);

	//�}�E�X��\��
	//CInput::ShowCursor(false);
	CInput::SetMouseInside(true);
	CInput::Update();
	CInput::Update();

	CLight::SetFogParam(CVector4D(1, 1, 1, 1), 1200, 1400);

	//�J����������
	CCamera::GetCamera()->LookAt(CVector3D(5, 5, 5),
		CVector3D(0, 0, 0),
		CVector3D(0.0, 1.0, 0.0));

	SetCurrentDirectory("data");

	CLight::SetType(0, CLight::eLight_Spot);

	//�|�X�g�G�t�F�N�g�𐶐�		��ʉ𑜓x,��ʊE�[�x�I�t�Z�b�g
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
	//�������̖��߂�����
	//�Q�[���N�����Ɉ�x�����Ă΂��
	//-----------------------------------------------------

	//imgui�K�{--------------------------------------------------------------------
// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//���{����͗p�t�H���g�w��
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

	ADD_RESOURCE("GameClear", CImage::CreateImage("UI/GameClear.png"));

	SOUND("���y")->Load("BGM/onngaku.wav", 1);
	SOUND("�^�C�g�����y")->Load("BGM/taitoruongaku.wav");
	SOUND("�����グ��")->Load("BGM/put_heavy_box.wav");
	SOUND("���ߍU��")->Load("BGM/�葫�E���ߏグ��.wav",1);
	SOUND("�����Ƃ�")->Load("BGM/sei_ge_fokusupun_otosu03.wav");
	SOUND("���p��")->Load("BGM/Cash_Register-Drawer02-1_Low_.wav");
	SOUND("���p��")->Load("BGM/baikyakutyu.wav");
	SOUND("����")->Load("BGM/arukuoto.wav");
	SOUND("��")->Load("BGM/Syaberukoe.wav");

	//Base::Add(new Game());
	Base::Add(new Title());
	//SOUND("�^�C�g�����y")->Volume(0.0);
	SOUND("�^�C�g�����y")->Play(true);
	
	
	
}


void Release()
{
	Base::ClearInstance();
	CLoadThread::ClearInstance();
	CSound::ClearInstance();
	CResourceManager::ClearInstance();
	// ImGui�̏I������
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

static void ResizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);

	//��ʉ𑜓x�ϓ�
	CCamera::GetCamera()->SetSize((float)w, (float)h);
	//��ʉ𑜓x�Œ�
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

//�t���X�N���[��?�E�C���h�E���[�h�̐؂�ւ�
//Alt+Enter�Ő؂�ւ���
void CheckFullScreen() {
	static int key_enter = 0;
	int key_enter_buf = key_enter;
	if (key_enter_buf ^ (key_enter = glfwGetKey(GL::window, GLFW_KEY_ENTER)) && key_enter && (glfwGetKey(GL::window, GLFW_KEY_LEFT_ALT) || glfwGetKey(GL::window, GLFW_KEY_RIGHT_ALT))) {
		GL::ChangeFullScreen(!GL::full_screen);
	}
}

int __main(int* argcp, char** argv) {
	// ���������[�N���o
	//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//�@.dll�̃p�X�w��
	//char path[256];
	//GetCurrentDirectory(256, path);
	//strcat_s(path, 255, "\\dll");
	//SetDllDirectory(path);

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

	Init();


	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER time_buf;
	QueryPerformanceCounter(&time_buf);
	float fps = 0;
	while (!glfwWindowShouldClose(GL::window)) {
		CheckFullScreen();
		//�e�o�b�t�@�[���N���A
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