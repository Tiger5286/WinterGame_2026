#include "Player.h"
#include "Resource/ResourceManager.h"

void Player::Init()
{
	m_pModel = ResourceManager::GetInstance().DuplicateModel(L"PlayerModel");
}

void Player::Update()
{
	
}

void Player::Draw() const
{
	m_pModel->Draw();
}
