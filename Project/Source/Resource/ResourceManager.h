#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

class Resource;
class Model;
class Graph;
class Font;

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
	void LoadModel(const std::wstring& filePath, const std::wstring& key);
	// 画像をロードする
	void LoadGraph(const std::wstring& filePath, const std::wstring& key);
	// 音をロードする
	//void LoadSound(const std::wstring& filePath, const std::wstring key);
	// フォントをロードする
	void LoadFont(const std::wstring& fontName, const std::wstring& key, int size);

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
	/// モデルを直接取得する
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>モデルのポインタ</returns>
	Model* GetModel(const std::wstring& key);

	/// <summary>
	/// モデルの複製を作成する
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>複製されたモデルのハンドル</returns>
	std::unique_ptr<Model> DuplicateModel(const std::wstring& key);

	/// <summary>
	/// 画像のハンドルを取得する
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>画像のハンドル</returns>
	Graph* GetGraph(const std::wstring& key);

	/// <summary>
	/// フォントハンドルを取得する
	/// </summary>
	/// <param name="key">登録名</param>
	/// <returns>フォントハンドル</returns>
	Font* GetFont(const std::wstring& key);

	/// <summary>
	/// リソースをCSVからロードする。この関数でロードしたリソースはファイルパスと紐づけられ、DeleteFromCSV()でまとめて削除できる
	/// </summary>
	/// <param name="filePath">csvファイルのパス</param>
	void LoadFromCSV(const std::wstring& filePath);

	/// <summary>
	/// LoadFromCSV()でロードしたリソースをまとめて削除する
	/// </summary>
	/// <param name="filePath">csvファイルのパス</param>
	void DeleteFromCSV(const std::wstring& filePath);

private:
	std::unordered_map<std::wstring,std::unique_ptr<Resource>> m_resources;
	std::unordered_map<std::wstring, std::vector<std::wstring>> m_csvResourceMap;
};