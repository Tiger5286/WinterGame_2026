#pragma once

class HitboxCollider
{
public:
	HitboxCollider();
	virtual ~HitboxCollider();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

};