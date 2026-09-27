#include "ResourceManager.h"
#include <cassert>
#include "DxLib.h"
#include "Model.h"
#include "Graph.h"

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
	if (m_Resources.find(key) != m_Resources.end())
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
	m_Resources[key] = std::make_unique<Model>(handle);
}

void ResourceManager::LoadGraph(const std::wstring& filePath, const std::wstring& key)
{
	// キーが重複していたらエラー
	if (m_Resources.find(key) != m_Resources.end())
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
	m_Resources[key] = std::make_unique<Graph>(handle);
}

void ResourceManager::Delete(const std::wstring& key)
{
	m_Resources.erase(key);
}

void ResourceManager::DeleteAll()
{
	m_Resources.clear();
}

Model* ResourceManager::GetModel(const std::wstring& key)
{
	auto it = m_Resources.find(key);
	if (it == m_Resources.end())
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
	auto it = m_Resources.find(key);
	if (it == m_Resources.end())
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
	auto it = m_Resources.find(key);
	if (it == m_Resources.end())
	{
		assert(false && "ResourceManager::GetGraph() : キーに対応したリソースが見つかりませんでした");
		return nullptr;
	}

	auto* pGraph = dynamic_cast<Graph*>(it->second.get());
	assert(pGraph && "ResourceManager::GetGraph() : キーに登録されているリソースはGraphではありません");
	return pGraph;
}
