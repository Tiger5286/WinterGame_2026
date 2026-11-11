#pragma once
#include <memory>

class UIBase
{
public:
	UIBase(int layer);
	virtual ~UIBase();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsVisible() const { return m_isVisible; }
	void SetIsVisible(bool isVisible) { m_isVisible = isVisible; }
	int GetLayer() const { return m_layer; }

protected:
	bool m_isVisible = true;
	// UIの描画順を決めるためのレイヤー(増えるほど手前に描画される)
	int m_layer = 0;
};