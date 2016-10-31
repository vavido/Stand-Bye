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
#include "SettingsProvider.h"
#include "SystemAccess.h" // To avoid cross linking

using namespace System;
using namespace System::IO; //Path.Combine()

SettingsProvider::SettingsProvider() {
	//Load settings
	if(loadSettings()) {
		//Checks if all settings are loaded
		checkSettingsFile();
	} else {
		LOG("Settingsfile could not be opend!");
		throw("SettingsFile could not be opened!");
	}
};

SettingsProvider::~SettingsProvider() {
	SettingsList->Clear();
	SettingsList->TrimExcess();
	LOG("Settingsprovider destroyed");
}

String^ SettingsProvider::getRawSetting(SettingName name) {
	return SettingsProvider::getSettingbyName(name)->GetValue()[0];
}

int SettingsProvider::getThreshold(SettingName name) {
	if(static_cast<int>(name) < 5) { //0 to 4 are threshold enumerators
		return System::Convert::ToInt32(SettingsProvider::getRawSetting(name));
	} else {
		LOG("The number of the enumerator should be between 0 and 4");
		throw("No Threshold with this name could be found!");
	}
};

bool SettingsProvider::isActive(SettingName name) {
	if(static_cast<int>(name) > 7) {
		return (SettingsProvider::getRawSetting(name) == "TRUE");
	} else {
		LOG("The name should be greater than 6!");
		throw("No valid name entered! Could not convert to boolean!");
	}
}

void SettingsProvider::setActiveState(SettingName name, const bool status) {
	String^ value = "ERROR";
	if(status == true) {
		value = "TRUE";
	} else {
		value = "FALSE";
	}
	getSettingbyName(name)->ChangeValue(value);
}

void SettingsProvider::setSetting(SettingName name, String^ value) {
	getSettingbyName(name)->ChangeValue(value);
};

void SettingsProvider::setSetting(SettingName name, const int value) {
	SettingsProvider::setSetting(name, Convert::ToString(value));
};

List<String^>^ SettingsProvider::getProcessList() {
	Setting ^proc = SettingsProvider::getSettingbyName(SettingName::PROC_EXCP);
	if(proc == nullptr) {
		return nullptr;
	}
	return proc->GetValue();
};

bool SettingsProvider::addProcessToProcessList(String^ process) {
	if(!getSettingbyName(SettingName::PROC_EXCP)->Contains(process)) {
		getSettingbyName(SettingName::PROC_EXCP)->AddValue(process);
		return true;
	} else {
		LOG("Process already added");
		return false;
	}
};

void SettingsProvider::removeProcessFromProcessList(String^ process) {
	getSettingbyName(SettingName::PROC_EXCP)->RemoveValue(process);
};

bool SettingsProvider::checkSettingsFile() {
	//Resets to DEFAULT values

	using namespace System::Collections;

	List<Setting^>^ default_list = gcnew List<Setting^>();

	default_list->Add(gcnew Setting(SettingName::WAIT_TIME, WAIT_TIME_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::LANGUAGE, "system"));
	default_list->Add(gcnew Setting(SettingName::USE_CPU, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::MAX_CPU, MAX_CPU_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::USE_RAM, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::MAX_RAM, MAX_RAM_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::USE_HDD, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::MAX_HDD, MAX_HDD_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::USE_NET, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::MAX_NET, MAX_NET_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::CHECK_SOUND, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::PROC_EXCP, PROC_EXCP_DEFAULT));
	default_list->Add(gcnew Setting(SettingName::SEARCH_UPDATES, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::SHOW_MESSAGES, "TRUE"));
	default_list->Add(gcnew Setting(SettingName::STANDBY_MODE, "SUSPEND"));
	default_list->Add(gcnew Setting(SettingName::LOGGING, "FALSE"));
	default_list->Add(gcnew Setting(SettingName::USE_SLEEPTIME, "FALSE"));
	default_list->Add(gcnew Setting(SettingName::SLEEPTIME, "00:00"));

	//Ensures that every setting is set
	for each(Setting^ default_setting in default_list) {
		bool settting_defined = false;
		for each(Setting^ defined_setting in SettingsList) {
			if(defined_setting->GetName() == default_setting->GetName()) {
				settting_defined = true;
			}
		}

		if(!settting_defined) {
			SettingsList->Add(default_setting);
			LOG("Settings provider added [" + default_setting->GetNameAsString() + "] Setting to file!");
			settingsFileCorrected = true;
		}
	}

	if(settingsFileCorrected) {
		//The settings file was incomplete
		return writeSettingsFile();
	} else {
		//The settings file was complete
		return true;
	}
};

bool SettingsProvider::saveSettingsToFile() {
	return writeSettingsFile();
};

List<Setting^>^ SettingsProvider::getAllSettings() {
	return SettingsList;
}

bool SettingsProvider::isFirstStart() {
	return settingsFileCorrected;
}


bool SettingsProvider::writeSettingsFile() {

	StreamWriter^ writer;
	try {
		writer = gcnew StreamWriter(getSettingsFilePath());

		for each (Setting^ set in SettingsList) {
			String^ all_values = "";

			for each(String^ val in set->GetValue()) {
				all_values = all_values + "'" + val + "'" + ",";
			}
			if(all_values->Length > 0) {
				all_values = all_values->Substring(0, all_values->Length - 1);
			}
			//If Setting has no value
			if(all_values == "") {
				all_values = "''";
			}
			LOG("Written Setting ['" + set->GetNameAsString() + "'] with value [" + all_values + "]");

			writer->WriteLine(set->GetNameAsString() + "=" + all_values);
		}

		return true;

	} catch(Exception ^e) {

		LOG("SettingsFile could not be opened to write Settings!");
		LOG(e);

		return false;

	} finally{
		if(writer != nullptr) {
			writer->Close();
		}
	}

};

bool SettingsProvider::loadSettings() {

	String ^line, ^name, ^raw_values;
	String ^gsf = "'";
	SettingsList = gcnew List<Setting^>();

	try {
		StreamReader^ din = File::OpenText(getSettingsFilePath());

		int count = 0;
		while((line = din->ReadLine()) != nullptr) {

			array<String^>^ splitline = line->Split('=');
			name = splitline[0];

			//Should be 2 --> left & right side of =
			if(splitline->Length > 1) {
				raw_values = splitline[1];
			} else {
				LOG("Loading settings failed! Resetting file to Factory Settings!");
				return reset();
			}

			//Deletes "'"
			raw_values->Replace(gsf, "");

			array<String^> ^value = raw_values->Split(',');

			// Creates new Setting
			SettingsList->Add(gcnew Setting(name, gcnew List<String^>(value)));
			LOG("Loaded Setting ['" + name + "'] with value ['" + raw_values + "']");
		}

		din->Close();
		return true;

	} catch(Exception^ e) {

		LOG("SettingsFile could not be opened: ");

		if(dynamic_cast<FileNotFoundException^>(e))
			LOG("file '{0}' not found", getSettingsFilePath());
		else
			LOG("problem reading file '{0}'", getSettingsFilePath());

		return checkSettingsFile();
	}
}

bool SettingsProvider::reset() {
	SettingsList->Clear();
	SettingsList->Add(gcnew Setting(SettingName::WAIT_TIME, WAIT_TIME_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::USE_CPU, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::MAX_CPU, MAX_CPU_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::USE_RAM, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::MAX_RAM, MAX_RAM_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::USE_HDD, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::MAX_HDD, MAX_HDD_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::USE_NET, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::MAX_NET, MAX_NET_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::CHECK_SOUND, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::PROC_EXCP, PROC_EXCP_DEFAULT));
	SettingsList->Add(gcnew Setting(SettingName::SEARCH_UPDATES, "TRUE"));
	SettingsList->Add(gcnew Setting(SettingName::SHOW_MESSAGES, "TRUE"));
	return writeSettingsFile();
};


String^ SettingsProvider::getSettingsFilePath() {
	//Adds the Path to the Settings file
	String^ folder = Path::Combine(SystemAccess::getStandByeFolderPath(), "Settings.ini");
	LOG("Retuned Settings-File-Path: " + folder);
	return folder;
};

Setting^ SettingsProvider::getSettingbyName(SettingName name) {

	//Returns setting with equal name
	for each (Setting^ set in SettingsList) {
		if(set->GetName() == name) {
			return set;
		}
	}

	LOG("Invalid settingName entered!");
	LOG("Could not get Setting!");
	return nullptr;
};