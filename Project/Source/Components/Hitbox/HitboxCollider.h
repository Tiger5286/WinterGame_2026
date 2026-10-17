#pragma once

class HitboxCollider
{
public:
	static constexpr unsigned int kColor = 0xffff00;
	static constexpr unsigned int kWeakPointColor = 0xff0000;

public:
	HitboxCollider();
	virtual ~HitboxCollider();

	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

};