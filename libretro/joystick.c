// JOYSTICK.C - joystick support for WinX68k

#include "common.h"
#include "prop.h"
#include "joystick.h"
#include "winui.h"
#include "keyboard.h"

#include "libretro.h"
extern retro_input_state_t input_state_cb;

#ifndef MAX_BUTTON
#define MAX_BUTTON 32
#endif

char joyname[2][MAX_PATH];
char joybtnname[2][MAX_BUTTON][MAX_PATH];
BYTE joybtnnum[2] = {0, 0};

BYTE joy[2];
BYTE JoyKeyState;
BYTE JoyKeyState0;
BYTE JoyKeyState1;
BYTE JoyState0[2];
BYTE JoyState1[2];

// This stores whether the buttons were down. This avoids key repeats.
BYTE JoyDownState0;
BYTE MouseDownState0;

// This stores whether the buttons were up. This avoids key repeats.
BYTE JoyUpState0;
BYTE MouseUpState0;

BYTE JoyPortData[2];

int *r_joy;

void Joystick_Init(void)
{
	joy[0] = 1;  // active only one
	joy[1] = 0;
	JoyKeyState = 0;
	JoyKeyState0 = 0;
	JoyKeyState1 = 0;
	JoyState0[0] = 0xff;
	JoyState0[1] = 0xff;
	JoyState1[0] = 0xff;
	JoyState1[1] = 0xff;
	JoyPortData[0] = 0;
	JoyPortData[1] = 0;
}

void Joystick_Cleanup(void)
{

}

BYTE FASTCALL Joystick_Read(BYTE num)
{
	BYTE joynum = num;
	BYTE ret0 = 0xff, ret1 = 0xff, ret;

	if (Config.JoySwap) joynum ^= 1;
	if (joy[num]) {
		ret0 = JoyState0[num];
		ret1 = JoyState1[num];
	}

	if (Config.JoyKey)
	{
		if ((Config.JoyKeyJoy2)&&(num==1))
			ret0 ^= JoyKeyState;
		else if ((!Config.JoyKeyJoy2)&&(num==0))
			ret0 ^= JoyKeyState;
	}

	ret = ((~JoyPortData[num])&ret0)|(JoyPortData[num]&ret1);

	return ret;
}


void FASTCALL Joystick_Write(BYTE num, BYTE data)
{
	if ( (num==0)||(num==1) ) JoyPortData[num] = data;
}

void FASTCALL Joystick_Update(int is_menu, int key)
{
	BYTE ret0 = 0xff, ret1 = 0xff;
	BYTE mret0 = 0xff, mret1 = 0xff;
	int num = 0; //xxx only joy1
	static BYTE pre_ret0 = 0xff, pre_mret0 = 0xff;

	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))ret0 ^= JOY_RIGHT;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))ret0 ^= JOY_LEFT;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP) )ret0 ^= JOY_UP;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))ret0 ^= JOY_DOWN;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A) )ret0 ^= JOY_TRG1;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B) )ret0 ^= JOY_TRG2;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))ret0 ^= (JOY_LEFT | JOY_RIGHT);	// [RUN]JOY_TRG3;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START) )ret0 ^= (JOY_UP | JOY_DOWN);	// [SELECT]JOY_TRG4;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X) )ret0 ^= JOY_TRG1;//JOY_TRG5;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y) )ret0 ^= JOY_TRG2;//JOY_TRG6;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L))ret0 ^= JOY_TRG1;//JOY_TRG7;
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R) )ret0 ^= JOY_TRG2;//JOY_TRG8;

	JoyDownState0 = ~(ret0 ^ pre_ret0) | ret0;
	JoyUpState0 = (ret0 ^ pre_ret0) & ret0;
	pre_ret0 = ret0;

	MouseDownState0 = ~(mret0 ^ pre_mret0) | mret0;
	MouseUpState0 = (mret0 ^ pre_mret0) & mret0;
	pre_mret0 = mret0;

	// disable Joystick when software keyboard is active
	if (!is_menu && !Keyboard_IsSwKeyboard()) {
		JoyState0[num] = ret0;
		JoyState1[num] = ret1;
	}
}

BYTE get_joy_downstate(void)
{
	return JoyDownState0;
}
void reset_joy_downstate(void)
{
	JoyDownState0 = 0xff;
}
BYTE get_joy_upstate(void)
{
	return JoyUpState0;
}
void reset_joy_upstate(void)
{
	JoyUpState0 = 0x00;
}


