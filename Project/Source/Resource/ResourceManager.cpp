#include "ResourceManager.h"
#include <cassert>
#include "DxLib.h"
#include "Model.h"
#include "Graph.h"
#include "Font.h"
#include "Utility/MyLib.h"

ResourceManager& ResourceManager::GetInstance()
{
	static ResourceManager instance;
	return instance;
}

ResourceManager::~ResourceManager()
{
	DeleteAll();
}

void ResourceManager::LoadModel(const std::wstring& filePath, const std::wstring& key)
{
	// キーが重複していたらエラー
	if (m_resources.find(key) != m_resources.end())
	{
		assert(false && "ResourceManager::LoadModel() : すでに登録済みのキーを登録しようとしています");
		return;
	}
	// モデルをロードしてハンドルを取得
	const int handle = MV1LoadModel(filePath.c_str());
	// モデルを正しくロードできたかチェック
	if (handle == -1)
	{
		assert(false && "ResourceManager::LoadModel() : モデルのロードが正しく行われませんでした");
		return;
	}
	// modelクラスを生成してmapに保存
	m_resources[key] = std::make_unique<Model>(handle);
}

void ResourceManager::LoadGraph(const std::wstring& filePath, const std::wstring& key)
{
	// キーが重複していたらエラー
	if (m_resources.find(key) != m_resources.end())
	{
		assert(false && "ResourceManager::LoadGraph() : すでに登録済みのキーを登録しようとしています");
		return;
	}
	const int handle = DxLib::LoadGraph(filePath.c_str());
	if (handle == -1)
	{
		assert(false && "ResourceManager::LoadGraph() : 画像のロードが正しく行われませんでした");
		return;
	}
	m_resources[key] = std::make_unique<Graph>(handle);
}

void ResourceManager::LoadFont(const std::wstring& fontName, const std::wstring& key, int size)
{
	// キーが重複していたらエラー
	if (m_resources.find(key) != m_resources.end())
	{
		assert(false && "ResourceManager::LoadFont() : すでに登録済みのキーを登録しようとしています");
		return;
	}
	const int handle = DxLib::CreateFontToHandle(fontName.c_str(), size, -1);
	if (handle == -1)
	{
		assert(false && "ResourceManager::LoadFont() : フォントが正しく作られませんでした");
		return;
	}
	m_resources[key] = std::make_unique<Font>(handle);
}

void ResourceManager::Delete(const std::wstring& key)
{
	m_resources.erase(key);
}

void ResourceManager::DeleteAll()
{
	m_resources.clear();
	m_csvResourceMap.clear();
}

Model* ResourceManager::GetModel(const std::wstring& key)
{
	auto it = m_resources.find(key);
	if (it == m_resources.end())
	{
		assert(false && "ResourceManager::GetModel() : キーに対応したリソースが見つかりませんでした");
		return nullptr;
	}

	auto* pModel = dynamic_cast<Model*>(it->second.get());
	assert(pModel && "ResourceManager::GetModel() : キーに登録されているリソースはModelではありません");
	return pModel;
}

std::unique_ptr<Model> ResourceManager::DuplicateModel(const std::wstring& key)
{
	auto it = m_resources.find(key);
	if (it == m_resources.end())
	{
		assert(false && "ResourceManager::DuplicateModel() : キーに対応したリソースが見つかりませんでした");
		return nullptr;
	}

	int handle = MV1DuplicateModel(it->second.get()->GetHandle());

	if (handle == -1)
	{
		assert(false && "ResourceManager::DuplicateModel() : モデルの複製が正しく行われませんでした");
		return nullptr;
	}

	return std::make_unique<Model>(handle);
}

Graph* ResourceManager::GetGraph(const std::wstring& key)
{
	auto it = m_resources.find(key);
	if (it == m_resources.end())
	{
		assert(false && "ResourceManager::GetGraph() : キーに対応したリソースが見つかりませんでした");
		return nullptr;
	}

	auto* pGraph = dynamic_cast<Graph*>(it->second.get());
	assert(pGraph && "ResourceManager::GetGraph() : キーに登録されているリソースはGraphではありません");
	return pGraph;
}

Font* ResourceManager::GetFont(const std::wstring& key)
{
	auto it = m_resources.find(key);
	if (it == m_resources.end())
	{
		assert(false && "ResourceManager::GetFont() : キーに対応したリソースが見つかりませんでした");
		return nullptr;
	}
	auto* pFont = dynamic_cast<Font*>(it->second.get());
	assert(pFont && "ResourceManager::GetGraph() : キーに登録されているリソースはFontではありません");
	return pFont;
}

void ResourceManager::LoadFromCSV(const std::wstring& filePath)
{
	auto csvData = MyLib::LoadCSV(filePath);
	for (const auto& row : csvData)
	{
		const std::wstring& type = row[0];
		const std::wstring& key = row[1];
		const std::wstring& path = row[2];
		int size = -1;
		if (type == L"font" && row.size() >= 4)
		{
			size = std::stoi(row[3]);
		}

		if (type == L"model")
		{
			LoadModel(path, key);
			m_csvResourceMap[filePath].push_back(key);
		}
		else if (type == L"graph")
		{
			LoadGraph(path, key);
			m_csvResourceMap[filePath].push_back(key);
		}
		else if (type == L"font")
		{
			if (size == -1)
			{
				assert(false && "ResourceManager::LoadFromCSV() : フォントサイズが正しくありません");
			}
			LoadFont(path, key, size);
			m_csvResourceMap[filePath].push_back(key);
		}
		else
		{
			assert(false && "ResourceManager::LoadFromCSV() : CSVのResourceTypeが正しくありません");
			continue;
		}
	}
}

void ResourceManager::DeleteFromCSV(const std::wstring& filePath)
{
	auto it = m_csvResourceMap.find(filePath);
	if (it != m_csvResourceMap.end())
	{
		for (const auto& key : it->second)
		{
			Delete(key);
		}
		m_csvResourceMap.erase(filePath);
	}
	else
	{
		assert(false && "ResourceManager::DeleteFromCSV() : CSVのリソースが見つかりませんでした");
	}
}
