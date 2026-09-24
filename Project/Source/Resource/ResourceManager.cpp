#include "ResourceManager.h"
#include <cassert>
#include "DxLib.h"
#include "Model.h"
#include "Graph.h"

ResourceManager::~ResourceManager()
{
	DeleteAll();
}

void ResourceManager::LoadModel(const std::wstring& filePath, const std::wstring key)
{
	// キーが重複していたらエラー
	if (m_Resources.find(key) != m_Resources.end())
	{
		assert(false && "ResourceManager::LoadModel() : すでに登録済みのキーを登録しようとしています");
		return;
	}
	// モデルをロードしてハンドルを取得
	const int handle = MV1LoadModel(filePath.c_str());
	// modelクラスを生成してmapに保存
	auto model = std::make_unique<Model>(handle);
	m_Resources[key] = std::move(model);
}

void ResourceManager::LoadGraph(const std::wstring& filePath, const std::wstring key)
{
	// キーが重複していたらエラー
	if (m_Resources.find(key) != m_Resources.end())
	{
		assert(false && "ResourceManager::LoadGraph() : すでに登録済みのキーを登録しようとしています");
		return;
	}
	const int handle = DxLib::LoadGraph(filePath.c_str());
	auto graph = std::make_unique<Graph>(handle);
	m_Resources[key] = std::move(graph);
}

void ResourceManager::Delete(std::wstring& key)
{
	m_Resources.erase(key);
}

void ResourceManager::DeleteAll()
{
	m_Resources.clear();
}

Model ResourceManager::DuplicateModel(std::wstring& key)
{
	return Model(-1);
}