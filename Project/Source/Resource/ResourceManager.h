#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Handle.h"
#include "Model.h"

class Resource;

/// <summary>
/// リソースを管理するシングルトンクラス
/// </summary>
class ResourceManager
{
public:
	static ResourceManager& GetInstance();
	virtual ~ResourceManager();

private:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager() = default;
public:

	// モデルをロードする
	void LoadModel(const std::wstring& filePath, const std::wstring key);
	// 画像をロードする
	void LoadGraph(const std::wstring& filePath, const std::wstring key);
	//void LoadSound(const std::wstring& filePath, const std::wstring key);

	/// <summary>
	/// リソースを解放する
	/// </summary>
	/// <param name="key">登録名</param>
	void Delete(const std::wstring& key);

	/// <summary>
	/// 全てのリソースを解放する
	/// </summary>
	void DeleteAll();

	/// <summary>
	/// モデルの複製を作成する
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>複製されたモデルのハンドル</returns>
	std::unique_ptr<Model> DuplicateModel(const std::wstring& key);

private:
	std::unordered_map<std::wstring,std::unique_ptr<Resource>> m_Resources;
};