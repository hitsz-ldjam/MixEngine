#pragma once
#ifndef MX_KEYCODE_H_
#define MX_KEYCODE_H_

#include "../Math/MxVector2.h"

namespace Mix {
#define MX_BC_MOUSE_FLAG 0x80000000
#define MX_BC_GAMEPAD_FLAG 0x40000000

#define MX_BC_KEYBOARD_CODE_START 1
#define MX_BC_MOUSE_CODE_START 120
#define MX_BC_GAMEPAD_CODE_START 140

#define MX_BC_CHECK_KEYBOARD(x) (((static_cast<uint32_t>(x)) >> 24) == 0)
#define MX_BC_CHECK_MOUSE(x) ((static_cast<uint32_t>(x)) & MX_BC_MOUSE_FLAG)
#define MX_BC_CHECK_GAMEPAD(x) ((static_cast<uint32_t>(x)) & MX_BC_GAMEPAD_FLAG)

#define MX_KEYCODE_GET_INDEX(x, start) ((static_cast<uint32_t>(x) & 0xFFFF)-(start))

#define MX_BC_KEYBOARD_INDEX(x) (MX_KEYCODE_GET_INDEX((x),MX_BC_KEYBOARD_CODE_START))
#define MX_BC_MOUSE_INDEX(x) (MX_KEYCODE_GET_INDEX((x),MX_BC_MOUSE_CODE_START))
#define MX_BC_GAMEPAD_INDEX(x) (MX_KEYCODE_GET_INDEX((x),MX_BC_GAMEPAD_CODE_START))
	/**
	 * \brief We use these enumerations to represent all possible inputs,
	 *		  including input from the keyboard, mouse, and gamepad.
	 *
	 *	\note We only use the low 2 bytes to represent different codes, and the high 2 bytes to set different flags.
	 */
	enum class ButtonCode :uint32_t {
		Unknown,

		// Character Keys
		A = MX_BC_KEYBOARD_CODE_START,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		// Arrow Keys
		Left,
		Up,
		Right,
		Down,

		// Numbers 
		/**< Numbers above the alphabet characters, below the functions keys on a traditional QWERTY keyboard */
		Num_1,
		Num_2,
		Num_3,
		Num_4,
		Num_5,
		Num_6,
		Num_7,
		Num_8,
		Num_9,
		Num_0,

		/**< These keys are on numpad */
		Np_1,
		Np_2,
		Np_3,
		Np_4,
		Np_5,
		Np_6,
		Np_7,
		Np_8,
		Np_9,
		Np_0,
		Np_Add,
		Np_Minus,
		Np_Multiply,
		Np_Divide,
		Np_Enter,
		Np_Decimal,

		Backspace,
		Tab,
		Enter,
		Pause,
		Numlock,
		ScrollLock,
		Capslock,
		Escape,
		Space,

		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,

		// Function Keys
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		LCtrl,
		RCtrl,
		LShift,
		RShift,
		LAlt,
		RAlt,
		LGUI,
		RGUI,

		Semicolon,
		Equals,
		Comma,
		Minus,
		Period,
		Slash,
		Backquote,
		LeftBracket,
		RightBracket,
		Backslash,
		Quote,

		Keyboard_EnumBegin = A,
		Keyboard_EnumEnd = Quote + 1,

		// Mouse Button		  
		Mouse_Left = MX_BC_MOUSE_CODE_START | MX_BC_MOUSE_FLAG,
		Mouse_Right,
		Mouse_Middle,
		Mouse_X1,
		Mouse_X2,

		Mouse_EnumBegin = Mouse_Left,
		Mouse_EnumEnd = Mouse_X2 + 1,


		//////////////////////////////////////////////////////////////////////////////////////////////
		//					   --------								   --------                     //
		//					 /          \ __________________________ /    __    \                   //
		//					|	____		                             | Y|    |                  //
		//			       /  /	 __	 \     	          --             __   --   __ \                 //
		//				  |  |	|__|  |	      [<]    \__/    [>]    | X|      | B| |                //
		//			     /	  \ ____ /				     		     --   __   --   \               //
		//			    |			  	   _				    ____	 | A|        |              //
		//			   /			    __| |__	              /  __  \    --          \             //
		//			  |                |__   __|             |  |__|  |                |            //
		//			 /                    |_|                 \ ____ /                  \           //
		//          |					                                                 |          //
		//		    |				  _--------------------------------_                 |          //
		//		    |               /                                    \               |          //
		//			|			   /                                      \				 |          //
		//			|            --                                        --            |          //
		//			 \			/                                            \          /           //
		//			   -------                                                  -------             //
		//                                                                                          //
		//			                                                                                //
		//                    These enumerations represent input from gamepad                       //
		//////////////////////////////////////////////////////////////////////////////////////////////

		Gamepad_A = MX_BC_GAMEPAD_CODE_START | MX_BC_GAMEPAD_FLAG,
		Gamepad_B,
		Gamepad_X,
		Gamepad_Y,
		Gamepad_LB,
		Gamepad_RB,
		Gamepad_LS,
		Gamepad_RS,
		Gamepad_Dpad_Left,
		Gamepad_Dpad_Up,
		Gamepad_Dpad_Right,
		Gamepad_Dpad_Down,

		Gamepad_SpecialLeft,
		Gamepad_SpecialRight,

		Gamepad_EnumBegin = Gamepad_A,
		Gamepad_EnumEnd = Gamepad_SpecialRight + 1,

		KeyboardCount = Keyboard_EnumEnd - Keyboard_EnumBegin,
		MouseCount = Mouse_EnumEnd - Mouse_EnumBegin,
		GamepadCount = Gamepad_EnumEnd - Gamepad_EnumBegin,
		EnumCount = KeyboardCount + MouseCount + GamepadCount + 1 // Add Unknown
	};

#define MX_AC_MOUSE_FLAG 0x80000000
#define MX_AC_GAMEPAD_FLAG 0x40000000
#define MX_AC_MOUSE_START 1
#define MX_AC_GAMEPAD_START 10

#define MX_AC_CHECK_MOUSE(x) ((static_cast<uint32_t>(x)) & MX_AC_MOUSE_FLAG)
#define MX_AC_CHECK_GAMEPAD(x) ((static_cast<uint32_t>(x)) & MX_AC_GAMEPAD_FLAG)

#define MX_AC_MOUSE_INDEX(x) (MX_KEYCODE_GET_INDEX((x),MX_AC_MOUSE_START))
#define MX_AC_GAMEPAD_INDEX(x) (MX_KEYCODE_GET_INDEX((x),MX_AC_GAMEPAD_START))

	enum class AxisCode :uint32_t {
		Unknown,

		Mouse_X = MX_AC_MOUSE_START | MX_AC_MOUSE_FLAG,
		Mouse_Y,
		Mouse_WheelX,
		Mouse_WheelY,
		Mouse_EnumBegin = Mouse_X,
		Mouse_EnumEnd = Mouse_WheelY + 1,

		Gamepad_LeftX = MX_AC_GAMEPAD_START | MX_AC_GAMEPAD_FLAG,
		Gamepad_LeftY,
		Gamepad_RightX,
		Gamepad_RightY,
		Gamepad_LeftTrigger,
		Gamepad_RightTrigger,
		Gamepad_EnumBegin = Gamepad_LeftX,
		Gamepad_EnumEnd = Gamepad_RightTrigger + 1,

		MouseAxisCount = Mouse_EnumEnd - Mouse_EnumBegin,
		GamepadAxisCount = Gamepad_EnumEnd - Gamepad_EnumBegin,
		EnumCount = MouseAxisCount + GamepadAxisCount + 1
	};


	std::string ToString(ButtonCode _button);
	std::string ToString(AxisCode _axis);
}

#endif
