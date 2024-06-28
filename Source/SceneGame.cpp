#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EffectManager.h"
#include <DirectXMath.h>
#include "Input/Input.h"
#include "StageManager.h"
#include "StageMain.h"

// ������
void SceneGame::Initialize()
{
	//�X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageManager.Register(stageMain);

	player = new Player();

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);
	//�J�����R���g���[���[������
	cameraController = new CameraController();
	for (int i = 0; i < 2; ++i)
	{
		EnemyManager& enemyManager = EnemyManager::Instance();
		EnemySlime* slime = new EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(slime);
	}
	//�Q�[�W�X�v���C�g
	gauge = new Sprite();
}

// �I����
void SceneGame::Finalize()
{
	EnemyManager::Instance().Clear();
	//�Q�[�W�X�v���C�g�I����
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;
	}
	//�J�����R���g���[���[�I����
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	//�X�e�[�W�I����
	StageManager::Instance().Clear();

	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}

// �X�V����
void SceneGame::Update(float elapsedTime)
{
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);
	//�X�e�[�W�X�V����
	StageManager::Instance().Update(elapsedTime);

	player->Update(elapsedTime);

	EnemyManager::Instance().Update(elapsedTime);

	EffectManager::Instance().Update(elapsedTime);
}

// �`�揈��
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// �`�揈��
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ���C�g�����i�������j

	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// 3D���f���`��
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);
		//�X�e�[�W�`��
		StageManager::Instance().Render(dc, shader);

		player->Render(dc, shader);

		EnemyManager::Instance().Render(dc, shader);
		shader->End(dc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::Instance().Render(rc.view, rc.projection);
	}
	// 3D�f�o�b�O�`��
	{
		//�v���C���[�f�o�b�O�v���~�e�B�u�`��
		player->DrawDebugPrimitive();
		//�G�l�~�[�f�o�b�O�v���~�e�B�u�`��
		EnemyManager::Instance().DrawDebugPrimitive();
		// ���C�������_���`����s
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// �f�o�b�O�����_���`����s
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2D�X�v���C�g�`��
	{
		RenderEnemyGauge(dc, rc.view, rc.projection);
	}

	// 2D�f�o�b�OGUI�`��
	{
		player->DrawDwbugGUI();
	}
}

//�G�l�~�[HP�Q�[�W�`��
void SceneGame::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//�ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	//�S�Ă̓G�ɓ����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		//�G�l�~�[�̓���̃��[���h���W(�G�̈ʒu)
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		//y�̈ʒu��������������
		worldPosition.y += enemy->GetHeight();
		
		//�G�̈ʒu��XMVECTOR�ɕϊ�
		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
		//���[���h���W(3D)����X�N���[�����W(2D)�֕ϊ�
		DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
			WorldPosition,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			Projection, View, World);

		//�X�N���[�����W(3D����2D�ɂ���XMVECTOR��XMFLOAT2�ɕϊ�)
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		//�Q�[�W�̒���
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		//�Q�[�W�̒������v�Z(���݌��N���/�ő匒�N���)
		float healthRate = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());
		//�Q�[�W�`��
		gauge->Render(dc,
			screenPosition.x - gaugeWidth / 2,//sprite�̍��ʒu(�G��x�ʒu-�Q�[�W���̔���)
			screenPosition.y - gaugeHeight,//Sprite�̏�ʒu(�G��y���W-�Q�[�W����)
			gaugeWidth * healthRate,//Sprite�̕�(�Q�[�W�̕�*���N��Ԕ䗦)
			gaugeHeight,//Sprite�̍���(�Q�[�W�̍���)
			0.0f, 0.0f,//Teture�̐؂���ʒuXY
			gaugeWidth,//Teture�̐؂��蕝
			gaugeHeight,//Teture�̐؂��荂��
			0.0f,//Sprite�̊p�x
			1.0f, 0.0f, 0.0f, 1.0f//Spriteno�F(r,g,b,a)
		);
	}

	//�G�l�~�[�z�u����
	Mouse& mouse = Input::Instance().GetMouse();
	if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
	{
		//�}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse.GetPositionX());
		screenPosition.y = static_cast<float>(mouse.GetPositionY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;
		//���C�̎n�_���v�Z
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
		//�n�_��2D����3D�ɕϊ�
		WorldPosition = DirectX::XMVector3Unproject(ScreenPosition,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			Projection,
			View, World);
		//���[���h��ԏ�ł̃��C�̎n�_(WorldPosition��XMFLOAT3�ɕϊ�)
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);
		//���C�̏I�_���Z�o
		screenPosition.z = 1.0f;	//�r���[�|�[�g�̐[�x��1.0�ɐݒ�
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);//screenPosition��XMVECTOR�ɕϊ�
		//�n�_��2D����3D�ɕϊ�
		WorldPosition = DirectX::XMVector3Unproject(ScreenPosition,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			Projection,
			View, World);
		//���[���h��ԏ�ł̃��C�̏I�_(WorldPosition��XMFLOAT3�ɕϊ�)
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		//���C�L���X�g(��ō�������C�ƃX�e�[�W�Ƃ̏Փ�)
		HitResult hit;
		
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			//�G��z�u(�G�𐶐�)
			EnemySlime* slime = new EnemySlime();
			//�G�̈ʒu�����C�ƃq�b�g�����ʒu�ɐݒ�
			slime->SetPosition(hit.position);
			//�G�Ǘ��ɓo�^
			EnemyManager::Instance().Register(slime);
		}
	}
}
