//////////////////////////////////////////////////////////////////////////
/*!
 * STAND-BYE! SOURCE CODE
 * ----------------------------------------------------------------------
 * for more information see: http://www.stand-bye.de or https://github.com/flobaader/Stand-Bye
 * Author: Florian Baader
 * Contact: contact@stand-bye.de
 * Copyright (c) 2016 Florian Baader, Stephan Le, Matthias Weirich
*/
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <algorithm>

using System::String;
using System::Collections::Generic::List;

enum class SettingName {
	MAX_CPU, MAX_HDD, MAX_NET, MAX_RAM, WAIT_TIME, PROC_EXCP, NET_ADAPT, LANGUAGE, STANDBY_MODE, SLEEPTIME, USE_CPU, USE_HDD, USE_NET, USE_RAM, CHECK_SOUND, SEARCH_UPDATES, SHOW_MESSAGES, LOGGING, USE_SLEEPTIME
};

ref class Setting {

private:
	List<String^>^ NAME_STRINGS = gcnew List<String^>();
	List<String^>^ value;
	SettingName name;

public:
	Setting(String^ name, List<String^>^ values);
	Setting(SettingName name, List<String^>^ values);
	Setting(SettingName name, String^ value);
	SettingName GetName();
	~Setting() {
		NAME_STRINGS->Clear();
		NAME_STRINGS->TrimExcess();
		value->Clear();
		value->TrimExcess();
	}
	List<String^>^ GetValue();
	void ChangeValue(List<String^>^ values_changed);
	void ChangeValue(String^ value_changed);
	void AddValue(String^ value_new);
	void RemoveValue(String^ value_remove);
	bool Contains(String^ value_find);
	String^ GetNameAsString();

private:
	String^ ConvertSettingNameToString(SettingName n);
	SettingName ConvertStringToSettingName(String^ name);
};
