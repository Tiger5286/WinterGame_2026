#include "MyLib.h"
#include "DxLib.h"

float MyLib::GetAngleDiff(float angle1, float angle2)
{
	float diff = angle1 - angle2 - DX_TWO_PI_F;
	while (diff > DX_PI_F) diff -= DX_TWO_PI_F;
	while (diff < -DX_PI_F) diff += DX_TWO_PI_F;
	return diff;
}
