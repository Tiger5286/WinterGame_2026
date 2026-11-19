#pragma once
class KeyboardInput
{
public:
	static KeyboardInput& GetInstance();
	~KeyboardInput() = default;

private:
	KeyboardInput(const KeyboardInput&) = delete;
	KeyboardInput& operator=(const KeyboardInput&) = delete;
	KeyboardInput() = default;

public:

	void Update();

	/// <summary>
	/// 指定のキーが押されているかどうか取得する
	/// </summary>
	/// <param name="keyInput">取得したいキー(KEY_INPUT_Zなど)</param>
	/// <returns>押されているかどうか</returns>
	bool IsPressed(int keyInput);

	/// <summary>
	/// 指定のキーが押された瞬間を取得する
	/// </summary>
	/// <param name="keyInput">取得したいキー(KEY_INPUT_Zなど)</param>
	/// <returns>押された瞬間true</returns>
	bool IsTriggerd(int keyInput);

	/// <summary>
	/// 指定のキーが離された瞬間を取得する
	/// </summary>
	/// <param name="keyInput">取得したいキー(KEY_INPUT_Zなど)</param>
	/// <returns>離された瞬間</returns>
	bool IsReleased(int keyInput);

private:
	char m_nowKeyInput[256] = {};
	char m_prevKeyInput[256] = {};
};