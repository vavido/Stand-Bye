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
#include "stdafx.h"
#include "Setting.h"

using namespace System;

String^ Setting::ConvertSettingNameToString(SettingName settingname) {
	return (String^)(NAME_STRINGS[(int)settingname]);
};

SettingName Setting::ConvertStringToSettingName(String^ string_name) {
	int counter = 0;
	for each(String^ s in NAME_STRINGS) {
		if (s == string_name) {
			SettingName n = static_cast<SettingName>(counter);
			return n;
		}
		counter++;
	}
	//The name was not valid
	LOG("No valid name entered!");
	throw("Failed to convert String^ to SettingName. No valid String^ entered!");
}

Setting::Setting(String^ name, List<String^>^ value) {
	this->name = Setting::ConvertStringToSettingName(name);
	this->value = value;
}

Setting::Setting(SettingName name, List<String^>^ value) {
	this->name = name;
	this->value = value;
}

Setting::Setting(SettingName name, String^ value) {
	this->name = name;
	this->value = gcnew List<String^>();
	this->value->Add(value);
}

SettingName Setting::GetName() {
	return name;
}

List<String^>^ Setting::GetValue() {
	return value;
}

void Setting::ChangeValue(List<String^>^ value_changed) {
	value = value_changed;
}

void Setting::ChangeValue(String^ value_changed) {
	value = gcnew List<String^>(1);
	value->Add(value_changed);
}

void Setting::AddValue(String^ value_new) {
	value->Add(value_new);
}

void Setting::RemoveValue(String^ value_remove) {
	value->Remove(value_remove);
}

bool Setting::Contains(String^ value_find) {
	return value->Contains(value_find);
}

String^ Setting::GetNameAsString() {
	return Setting::ConvertSettingNameToString(this->name);
}