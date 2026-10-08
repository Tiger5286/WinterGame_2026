#include "UIManager.h"
#include "Game/UI/UIBase.h"

UIManager& UIManager::GetInstance()
{
	static UIManager instance;
	return instance;
}

void UIManager::Init()
{
	m_uiList.clear();
	m_minLayer = 0;
	m_maxLayer = 0;
}

void UIManager::Update()
{
	// UIの更新処理を行う
	std::list<std::shared_ptr<UIBase>> removeList;
	for (auto& ui : m_uiList)
	{
		ui->Update();
		if (!ui->IsAlive())
		{
			removeList.push_back(ui);
		}
	}
	// 生存していないUIをリストから削除する
	for (auto& ui : removeList)
	{
		m_uiList.remove(ui);
	}
}

void UIManager::Draw()
{
	// レイヤー順に描画する
	for (int i = m_minLayer; i <= m_maxLayer; i++)
	{
		// レイヤーが一致するUIを描画する
		for (auto& ui : m_uiList)
		{
			bool isVisible = ui->IsVisible();
			bool isMatchLayer = ui->GetLayer() == i;
			if (isVisible && isMatchLayer)
			{
				ui->Draw();
			}
		}
	}
}

void UIManager::AddUI(std::shared_ptr<UIBase> ui)
{
	// レイヤーの最低値と最大値を更新する
	int layer = ui->GetLayer();
	if (layer < m_minLayer) m_minLayer = layer;
	if (layer > m_maxLayer) m_maxLayer = layer;
	// UIをリストに追加する
	m_uiList.push_back(ui);
	ui->Init();
}
