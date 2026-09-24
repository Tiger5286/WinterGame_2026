#pragma once

/// <summary>
/// 変更不可のハンドルを持つクラス
/// </summary>
class Handle
{
public:
	// コンストラクタでハンドルを初期化
	Handle(int handle) :
		m_handle(handle)
	{
	}

	// ハンドルを取得
	int Get() const
	{
		return m_handle;
	}

private:
	const int m_handle;
};