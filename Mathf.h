#pragma once

//���������Z�p
class Mathf
{
public:
	//���`�ۊ�
	static float Lerp(float a, float b, float t)
	{
		return a * (1.0f - t) + (b * t);
	}

};
