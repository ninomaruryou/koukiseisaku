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
	// �M���G�b�W // �d�q�@��ɂ����āA�M���G�b�W�̓f�W�^���M���̃��[����n�C (0 ���� 1) �܂��̓n�C���烍�[ (1 ���� 0) �ւ̑J�ڂł��B
	// ���`�g�ł��邽�߃G�b�W�ƌĂ΂�܂��B�M���������̓_�ɃG�b�W�������Ƃ�\���܂��B
	// // �����オ��G�b�W�́A���[����n�C�ւ̑J�ڂł��B[1]����̓|�W�e�B�u�G�b�W�Ƃ��Ă΂�܂��B
	// ��H�������オ��G�b�W�Ńg���K�����ƁA�N���b�N�M�������[����n�C�ɕω�����Ƃ��ɃA�N�e�B�u�ɂȂ�A�n�C���烍�[�ւ̑J�ڂ͖�������܂��B
	// ����������G�b�W�͍������ւ̑J�ڂł��B[1]����̓l�K�e�B�u �G�b�W�Ƃ��Ă΂�܂��B
	// ��H������������G�b�W�Ńg���K�����ꍇ�A�N���b�N�M���� High ���� Low �ɕω�����Ƃ��ɃA�N�e�B�u�ɂȂ�ALow ���� High �ւ̑J�ڂ͖�������܂��B
	// // ���[�f�B���O �G�b�W�́A�p���X�̃t�����g �G�b�W�Ńg���K�[�����C�x���g�ł��B
	// �N���b�N�� t = 0 �Ŏn�܂�Ɖ��肷��ƁA�ŏ��̈ʒu�� t = 1 �Ńg���K�[����܂��B
	// // �g���[�����O �G�b�W�̓��[�f�B���O �G�b�W�̋t�ł��B�G�b�W�B�p���X�̃o�b�N �G�b�W�Ńg���K����܂��B
	// �N���b�N�� t = 0 �Ŏn�܂�Ɖ��肷��ƁA�ŏ��̈ʒu�� t = 0 �Ńg���K����܂��B
	// // �t�����g �G�b�W�܂��̓��[�f�B���O �G�b�W�A����уo�b�N �G�b�W�܂��̓g���[�����O �G�b�W�Ƃ����p��́A���ꂼ���������܂��B
	// �N���b�N �T�C�N�����̊֘A����G�b�W�̈ʒu�B
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
	struct state //��Ԑݒ�
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

