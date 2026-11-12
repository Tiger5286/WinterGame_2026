#pragma once
#include <memory>

class SceneManager;

class SceneBase : public std::enable_shared_from_this<SceneBase>
{
public:
	SceneBase(SceneManager& sceneManager);
	virtual ~SceneBase() = default;

	virtual void Load() {}

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() const = 0;

protected:
	void DrawGrid() const;

protected:
	SceneManager& m_sceneManager;
};

