#include "ClearScene.h"
#include "SpriteBasis.h"
#include "imgui.h"

ClearScene::ClearScene()
{
	TextureManager::GetInstance()->LoadTexture("Resources/clear.png");
	clearSprite_ = std::make_unique<Sprite>();
	clearSprite_->Initialize("Resources/clear.png");
	clearSprite_->SetSize(Vector2{ 1280.0f,720.0f });
	clearSprite_->SetTextureSize(Vector2{ 1280,720 });
	clearSprite_->SetColor({ 1,1,1,1 });
	globalVar_ = GlobalVariables::GetInstance();
}

void ClearScene::Init() 
{
	input_ = Input::GetInstance();
	phase_ = ClearPhase::kFadeIn;
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Status::FadeIn, globalVar_->GetFloatValue("global", "fadeTime(sec)"));
}

void ClearScene::Update()
{
	fade_->Update();
	clearSprite_->Update();

#ifdef _DEBUG
	ImGui::Begin("CLEAR");
	ImGui::End();
#endif // _DEBUG

	ChangePhase();
}

void ClearScene::Draw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	clearSprite_->Draw();
	fade_->Draw();
}

void ClearScene::ChangePhase()
{
	switch (phase_) 
	{
	case ClearPhase::kClear:
		if (input_->TriggerKey(DIK_SPACE)) 
		{
			phase_ = ClearPhase::kFadeOut;
			fade_->Start(Status::FadeOut, globalVar_->GetFloatValue("global", "fadeTime(sec)"));
		}
		break;
	case ClearPhase::kFadeIn:
		if (fade_->IsFinished())
		{
			phase_ = ClearPhase::kClear;
		}
		break;
	case ClearPhase::kFadeOut:
		if (fade_->IsFinished()) 
		{
			// シーンの切り替え依頼
			sceneNo_ = TITLE;
		}
		break;
	}
}
