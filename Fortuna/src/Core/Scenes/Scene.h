#pragma once

class Renderer;

enum class SceneType
{
	None,
	MainMenu,
	Game,
	Editor
};

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Update(float deltaTime) = 0;
	virtual void Render(Renderer* renderer) {}

	SceneType GetType() const { return type; }

	bool WantsToTransition() const { return next != type; }
	SceneType GetNextScene() const { return next; }

	void SetNextScene(SceneType _next)
	{
		next = _next;
	}

protected:
	void InitType(SceneType _type)
	{
		type = _type;
		next = type;
	}
private:
	SceneType type = SceneType::None;
	SceneType next = SceneType::None;
};