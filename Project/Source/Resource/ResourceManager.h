#pragma once
#include <unordered_map>
#include <string>
#include "Handle.h"

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

	// リソースデータの種類
	enum class Type
	{
		None,
		Model,
		Graph
	};

	// リソース一つのデータ
	struct Data
	{
		Handle handle;
		Type type = Type::None;
	};

	/// <summary>
	/// リソースを読み込む
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="key">登録名</param>
	/// <param name="type">ロードするデータの種類</param>
	void Load(std::wstring& filePath, std::wstring& key, Type type);

	/// <summary>
	/// リソースを解放する
	/// </summary>
	/// <param name="key">登録名</param>
	void Delete(std::wstring& key);

	/// <summary>
	/// 全てのリソースを解放する
	/// </summary>
	void DeleteAll();

	/// <summary>
	/// モデルの複製を作成し、ハンドルを返す(※複製されたモデルはこちら側で管理しないため、解放などは各自で行うこと)
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>複製されたモデルのハンドル</returns>
	Handle DuplicateModel(std::wstring& key);

private:
	// モデルを読み込む
	void LoadModel(std::wstring& filePath, std::wstring& key);
	// 画像を読み込む
	void LoadGraph(std::wstring& filePath, std::wstring& key);

private:
	std::unordered_map<Data, std::wstring> m_handles;
};