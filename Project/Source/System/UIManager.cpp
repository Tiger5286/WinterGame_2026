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
	// 持ち主が消えたUIをリストから外す
	m_uiList.remove_if([](const std::weak_ptr<UIBase>& ui) { return ui.expired(); });

	// UIの更新処理を行う
	for (auto& weakUI : m_uiList)
	{
		if (auto ui = weakUI.lock())
		{
			ui->Update();
		}
	}
}

void UIManager::Draw()
{
	// レイヤー順に描画する
	for (int i = m_minLayer; i <= m_maxLayer; i++)
	{
		// レイヤーが一致するUIを描画する
		for (auto& weakUI : m_uiList)
		{
			if (auto ui = weakUI.lock())
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
}

void UIManager::AddUI(const std::shared_ptr<UIBase>& ui)
{
	// レイヤーの最低値と最大値を更新する
	int layer = ui->GetLayer();
	if (layer < m_minLayer) m_minLayer = layer;
	if (layer > m_maxLayer) m_maxLayer = layer;
	// UIをリストに追加する
	m_uiList.push_back(ui);
	ui->Init();
}
