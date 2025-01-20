
#include "EffekseerManager.h"

EffekseerManager *EffekseerManager::m_instance = nullptr;

static const char model_unlit_vs_gl3[] = R"(#version 430
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint Index;
};

struct VS_Output
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel_Inst[10];
    vec4 fUV[10];
    vec4 fModelColor[10];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
centroid out vec4 _VSPS_Color;
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_PosP;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 mModel = CBVS0.mModel_Inst[index];
    vec4 uv = CBVS0.fUV[index];
    vec4 modelColor = CBVS0.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0));
    vec4 localPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = localPos * mModel;
    Output.PosVS = worldPos * CBVS0.mCameraProj;
    Output.Color = modelColor;
    vec2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    Input.Index = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_PosP = flattenTemp.PosP;
}

)";

static const char model_unlit_ps_gl3[] = R"(#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
};

struct PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
    vec4 miscFlags;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_depthTex;

centroid in vec4 _VSPS_Color;
centroid in vec2 _VSPS_UV;
in vec4 _VSPS_PosP;
layout(location = 0) out vec4 _entryPointOutput[2];

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 param = c;
    vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

vec4 LinearToSRGB(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

vec4 ConvertFromSRGBTexture(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
}

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec3 SRGBToLinear(vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

vec4 SRGBToLinear(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

vec4 ConvertToScreen(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return SRGBToLinear(param);
}

vec4 _main(PS_Input Input)
{
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.PosP = _VSPS_PosP;

    bool convertColorSpace = !(CBPS0.miscFlags.x == 0.0);
    vec4 param = texture(Sampler_sampler_colorTex, Input.UV);
    bool param_1 = convertColorSpace;
    vec4 Output = ConvertFromSRGBTexture(param, param_1) * Input.Color;
    vec3 _258 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_258.x, _258.y, _258.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * screenUV.y);
    if (!(CBPS0.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_2 = backgroundZ;
        float param_3 = screenPos.z;
        vec4 param_4 = CBPS0.softParticleParam;
        vec4 param_5 = CBPS0.reconstructionParam1;
        vec4 param_6 = CBPS0.reconstructionParam2;
        Output.w *= SoftParticle(param_2, param_3, param_4, param_5, param_6);
    }
    if (Output.w == 0.0)
    {
        discard;
    }
    vec4 param_7 = Output;
    bool param_8 = convertColorSpace;
    vec4 _363 = ConvertToScreen(param_7, param_8);


    _entryPointOutput[0] = _363;
    _entryPointOutput[1].w = Output.xyz * CBPS0.fEmissiveScaling.x;
}

)";

EffekseerManager::EffekseerManager() :
	Base(eEffectManager)
{
	//エフェクトの先読み
	struct EFK_DATA {
		std::string name;
		std::u16string efk;
	}
	efk_data[] = {
		{ "Lance3",u"Effect/MAGICALxSPIRAL/Lance3.efk" },
        { "Sylph3",u"Effect/MAGICALxSPIRAL/Sylph3.efk" },
        { "Blow4",u"Effect/tktk01/Blow4.efk" },
        { "Blow10",u"Effect/tktk01/Blow10.efk" },
		{ "Sword1",u"Effect/NextSoft01/Sword1.efk" },
        { "DragonBreath",u"Effect/NextSoft01/GreenBreath1.efk" },
        { "Jet_On",u"Effect/MAGICALxSPIRAL/FirePot1.efk" },
		{ "HealPotion2",u"Effect/NextSoft01/HealPotion2.efk" },
	};


	// Create a manager of effects
	// エフェクトのマネージャーの作成
	m_manager = ::Effekseer::Manager::Create(8000);

	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererGL::CreateGraphicsDevice(::EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	m_renderer = ::EffekseerRendererGL::Renderer::Create(graphicsDevice,8000);
	
	
	//std::shared_ptr<EffekseerRenderer::ExternalShaderSettings> shader_setting(new EffekseerRenderer::ExternalShaderSettings());
    //shader_setting->StandardShader = graphicsDevice->CreateShaderFromCodes({ model_unlit_vs_gl3 }, { model_unlit_ps_gl3 });
   // shader_setting->Blend = Effekseer::AlphaBlendType.Blend;
    //m_renderer->SetExternalShaderSettings(shader_setting);
	
	
	// Sprcify rendering modules
	// 描画モジュールの設定
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());																							
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	//右手座標系に設定
	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::RH);

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());
	m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());
	m_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	
	// 音再生用インスタンスの生成
	m_sound = EffekseerSound::Sound::Create(32);

	// 音再生用インスタンスから再生機能を指定
	m_manager->SetSoundPlayer(m_sound->CreateSoundPlayer());

	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	m_manager->SetSoundLoader(m_sound->CreateSoundLoader());


	//エフェクトの先読み
	unsigned int efk_data_size = sizeof(efk_data) / sizeof(efk_data[0]);
	for (int i = 0; i < efk_data_size; i++) {
		EFK_DATA *d = &efk_data[i];
		for(int i=0;i< m_inctane_size;i++)
			m_effect_list[d->name].m_effets.push_back(Effekseer::Effect::Create(m_manager, d->efk.c_str()));
	}
	
	if (!m_instance) m_instance = this;
}

EffekseerManager::~EffekseerManager()
{
	// エフェクトの停止
	m_manager->StopAllEffects();

	for (auto& effects : m_effect_list) {
		for (auto& e : effects.second.m_effets) {
			// エフェクトの破棄
		//	ES_SAFE_RELEASE(e);
		}
	}

	m_sound.Reset();
	m_renderer.Reset();
	m_manager.Reset();

	m_instance = nullptr;

}

void EffekseerManager::ClearInstance()
{
	if (!m_instance) return;
	EffekseerManager::GetInstance()->SetKill();
}
void EffekseerManager::Update() {
	// エフェクトの更新処理を行う
	//m_manager->Update();
	CVector3D pos = CCamera::GetCurrent()->GetPos();
	CVector3D at = pos + CCamera::GetCurrent()->GetDir();
	CVector3D up = CCamera::GetCurrent()->GetUp();
	m_sound->SetListener(Effekseer::Vector3D(pos.x, pos.y, pos.z), Effekseer::Vector3D(at.x, at.y, at.z), Effekseer::Vector3D(up.x, up.y, up.z));


	// Update the manager
	// マネージャーの更新
	Effekseer::Manager::UpdateParameter updateParameter;
	m_manager->Update(updateParameter);

}
void EffekseerManager::Render()
{
	

	if (CShadow::isDoShadow()) return;
	// 投影行列を設定
	::Effekseer::Matrix44 mat;
	memcpy(mat.Values, CCamera::GetCurrent()->GetProjectionMatrix().f, sizeof(float) * 16);
	m_renderer->SetProjectionMatrix(mat);
	memcpy(mat.Values, CCamera::GetCurrent()->GetViewMatrix().f, sizeof(float) * 16);
	// カメラ行列を設定
	m_renderer->SetCameraMatrix(mat);

	
	// Render effects
	// エフェクトの描画を行う。
	m_renderer->BeginRendering();
	Effekseer::Manager::DrawParameter drawParameter;
	drawParameter.ZNear = 0.0f;
	drawParameter.ZFar = 1.0f;
	drawParameter.ViewProjectionMatrix = m_renderer->GetCameraProjectionMatrix();
	// エフェクトの描画を行う。
	m_manager->Draw(drawParameter);

	// エフェクトの描画終了処理を行う。
	m_renderer->EndRendering();
}

Effekseer::EffectRef EffekseerManager::GetEffect(const std::string & name)
{
	Effect* e = &m_effect_list[name];
	Effekseer::EffectRef ret = e->m_effets[e->m_index];
	e->m_index = (e->m_index + 1) % e->m_effets.size();
	return ret;
}
