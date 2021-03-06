//////////////////////////////////////////////////////////////////////////
/*!
 * STAND-BYE! SOURCE CODE
 * ----------------------------------------------------------------------
 * for more information see: http://www.stand-bye.de or https://github.com/flobaader/Stand-Bye
 * Author: Florian Baader, Stephan Le
 * Contact: contact@stand-bye.de
 * Copyright (c) 2016 Florian Baader, Stephan Le, Matthias Weirich
*/
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "SettingsProvider.h"
#include <iostream>
#include <Psapi.h> //To get Processes
#include <stdio.h>//To get Processes
#include <tchar.h>//To get Processes
#include <lmcons.h> //To get user name


using namespace System::Diagnostics;
using namespace System::Collections::Generic;
using System::Runtime::InteropServices::DllImportAttribute;
using System::Windows::Forms::Timer;
using System::Windows::Forms::Application;
using System::Windows::Forms::PowerState;
struct IAudioMeterInformation; //To avoid cross linking
using Cassia::TerminalServicesManager;
using Cassia::ITerminalServer;
using Cassia::ITerminalServicesSession;

public ref class SystemAccess {
private:
	//Attributes
	PerformanceCounter^ perfCPU;
	PerformanceCounter^ perfHDD;
	List<PerformanceCounter^>^ perfNETs;
	SettingsProvider^ setprov;

	//Methods
	float getCPUUsage();
	float getRAMUsage();
	float getNETUsage();
	float getHDDUsage();

	void reloadNetworkAdapters();

public:
	SystemAccess(SettingsProvider^ p);
	~SystemAccess();

	enum class  SystemMetric : char { CPU, RAM, NETWORK, HDD, SOUND };

	float GetMetric(SystemMetric s);

	static void StartESM(SettingsProvider^);

	static List<String^> ^GetNetAdapterNames();

	static float GetLastInputTime();

	static List<String^>^ GetRunningProccesses();

	static void SetPresentationMode(boolean value);

	static void SetAutoStart(boolean value);

	static void EnableWindowsStandBy();

	static void DisableWindowsStandBy();

	static bool isUserActive();

	static bool IsInAutoStart();

	static float getAudioPeak();

	static List<String^>^ getLoggedInUsers();

	static String^ getActiveUser();

	static String^ getStartUserOfStandbye();

	static bool MultiUsersPreventStandby();

	static String^ getStandByeFolderPath();

	static System::Drawing::Bitmap^ getIconOfProcess(String^ path);

	static bool isPortable();

	static bool inDebugMode();

	[DllImport("user32.dll")]
	static bool GetLastInputInfo(LASTINPUTINFO* plii);
};
