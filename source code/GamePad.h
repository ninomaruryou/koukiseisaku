#pragma once

#include <Windows.h>
#include <xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")

#include <memory>

#include <windows.h>
#include <cstdint>

enum class trigger_mode { none, rising_edge, falling_edge };
class keybutton
{
	int vkey;
	int current_state;
	int previous_state;
public:
	keybutton(int vkey) : vkey(vkey), current_state(0), previous_state(0)
	{

	}
	virtual ~keybutton() = default;
	keybutton(keybutton&) = delete;
	keybutton& operator=(keybutton&) = delete;

	// Signal edge
	// In electronics, a signal edge is a transition in a digital signal either from low to high(0 to 1) or from high to low(1 to 0).It is called an edge because the square wave which represents a signal has edges at those points.
	// A rising edge is the transition from low to high.[1] It is also named positive edge.When a circuit is rising edge - triggered, it becomes active when its clock signal goes from low to high, and ignores the high - to - low transition.
	// A falling edge is the high to low transition.[1] It is also known as the negative edge.When a circuit is falling edge - triggered, it becomes active when the clock signal goes from high to low, and ignores the low - to - high transition.
	// A leading edge is an event that is triggered on the front edge of a pulse.Assuming a clock begins at t = 0, the first position would be triggered at t = 1.
	// A trailing edge is the opposite of a leading edge.It is triggered on the back edge of a pulse.Assuming the clock begins at t = 0, the first position would be triggered at t = 0.
	// The terms front edge or leading edge, and back edge or trailing edge describe the related position of edges in a clock cycle.
	// 信号エッジ // 電子機器において、信号エッジはデジタル信号のローからハイ (0 から 1) またはハイからロー (1 から 0) への遷移です。
	// 方形波であるためエッジと呼ばれます。信号がそれらの点にエッジを持つことを表します。
	// // 立ち上がりエッジは、ローからハイへの遷移です。[1]これはポジティブエッジとも呼ばれます。
	// 回路が立ち上がりエッジでトリガされると、クロック信号がローからハイに変化するときにアクティブになり、ハイからローへの遷移は無視されます。
	// 立ち下がりエッジは高から低への遷移です。[1]これはネガティブ エッジとも呼ばれます。
	// 回路が立ち下がりエッジでトリガされる場合、クロック信号が High から Low に変化するときにアクティブになり、Low から High への遷移は無視されます。
	// // リーディング エッジは、パルスのフロント エッジでトリガーされるイベントです。
	// クロックが t = 0 で始まると仮定すると、最初の位置は t = 1 でトリガーされます。
	// // トレーリング エッジはリーディング エッジの逆です。エッジ。パルスのバック エッジでトリガされます。
	// クロックが t = 0 で始まると仮定すると、最初の位置は t = 0 でトリガされます。
	// // フロント エッジまたはリーディング エッジ、およびバック エッジまたはトレーリング エッジという用語は、それぞれを説明します。
	// クロック サイクル内の関連するエッジの位置。
	bool state(trigger_mode trigger_mode)
	{
		previous_state = current_state;
		if (static_cast<USHORT>(GetAsyncKeyState(vkey)) & 0x8000)
		{
			current_state++;
		}
		else
		{
			current_state = 0;
		}
		if (trigger_mode == trigger_mode::rising_edge)
		{
			return previous_state == 0 && current_state > 0;
		}
		else if (trigger_mode == trigger_mode::falling_edge)
		{
			return previous_state > 0 && current_state == 0;
		}
		else
		{
			return current_state > 0;
		}
	}
};

class gamepad
{
private:
	enum class side { l, r };
	enum class axis { x, y };
	enum class deadzone_mode { independent_axes, circular, none };
public:
	enum class button { a, b, x, y, left_thumb, right_thumb, left_shoulder, right_shoulder, start, back, up, down, left, right, end };
private:
	static const size_t _button_count = static_cast<size_t>(button::end);

private:
	int user_id = 0;
	struct state //状態設定
	{
		DWORD packet;
		bool connected = false;
		bool buttons[_button_count];
		float triggers[2]; //[side]
		float thumb_sticks[2][2]; //[side][axis]
	};
	state current_state;
	state previous_state;

	// use on emulation_mode
	bool emulation_mode = false; //if gamepad is disconnected then keyboard is used but only user_id = 0
	std::unique_ptr<keybutton> button_keys[_button_count];
	std::unique_ptr<keybutton> thumb_stick_keys[2][4]; //[side][+x/-x/+y/-y]
	std::unique_ptr<keybutton> trigger_keys[2]; //[side]

public:
	gamepad(int user_id = 0, float deadzonex = 0.05f, float deadzoney = 0.02f, deadzone_mode deadzone_mode = deadzone_mode::independent_axes);
	virtual ~gamepad() = default;
	gamepad(gamepad const&) = delete;
	gamepad& operator=(gamepad const&) = delete;


	bool acquire();

	bool button_state(button button, trigger_mode trigger_mode = trigger_mode::rising_edge) const;

	float thumb_state_lx() const { return thumb_state(side::l, axis::x); }
	float thumb_state_ly() const { return thumb_state(side::l, axis::y); }
	float thumb_state_rx() const { return thumb_state(side::r, axis::x); }
	float thumb_state_ry() const { return thumb_state(side::r, axis::y); }

	float trigger_state_l()const { return trigger_state(side::l); }
	float trigger_state_r()const { return trigger_state(side::r); }

private:
	float thumb_state(side side, axis axis) const;
	float trigger_state(side side)const;

	float deadzonex = 0.05f;
	float deadzoney = 0.02f;
	deadzone_mode _deadzone_mode = deadzone_mode::none;
	void apply_stick_deadzone(float x, float y, deadzone_mode deadzone_mode, float max_value, float deadzone_size, _Out_ float& result_x, _Out_ float& result_y);
};

