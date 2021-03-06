//////////////////////////////////////////////////////////////////////////
/*!
 * STAND-BYE! SOURCE CODE
 * ----------------------------------------------------------------------
 * for more information see: http://www.stand-bye.de or https://github.com/flobaader/Stand-Bye
 * Author: Florian Baader, Florian Baader
 * Contact: contact@stand-bye.de
 * Copyright (c) 2016 Florian Baader, Stephan Le, Matthias Weirich
*/
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "SystemAccess.h"

#define EXIT_ON_ERROR(hr) \
if (FAILED(hr)){LOG("AUDIO LEVEL DETECTION FAILED"); return 0.0f;}

using System::Runtime::InteropServices::Marshal;

SystemAccess::SystemAccess(SettingsProvider^ p) {
	LOG("Loading SystemAccess instance...");
	setprov = p;

	//Enables autostart if first launch
	if(setprov->isFirstStart() && SystemAccess::isPortable()) {
		this->SetAutoStart(true);
		LOG("Enabled autostart on first launch of the application");
	}

	perfCPU = gcnew PerformanceCounter("Processor", "% Processor Time", "_Total");
	perfHDD = gcnew PerformanceCounter("PhysicalDisk", "Disk Bytes/sec", "_Total");
	reloadNetworkAdapters();
}

float SystemAccess::getCPUUsage() {
	//Note: performance counters need administrator privileges at Windows Vista
	float cpuUsage_percent = perfCPU->NextValue();
	return cpuUsage_percent;
}

float SystemAccess::getRAMUsage() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	ULONGLONG totalPhysMem = memInfo.ullTotalPhys;
	ULONGLONG usedPhysMem = totalPhysMem - memInfo.ullAvailPhys;
	return (float)(100.0 * usedPhysMem / totalPhysMem);
}

float SystemAccess::getNETUsage() {
	float kbytes_per_sec = 0;
	try {
		for each(PerformanceCounter^ perf in *perfNETs) {
			kbytes_per_sec += perf->NextValue() / 1000;
		}
	} catch(Exception^ e) {
		LOG("Error getting NET Status");
		LOG(e);
		LOG("Reloading Network Adapters!");
		reloadNetworkAdapters();
		return 0;
	}

	return kbytes_per_sec;
}

float SystemAccess::getHDDUsage() {
	float kbytes_per_sec = perfHDD->NextValue() / 1000;
	return kbytes_per_sec;
}

void SystemAccess::reloadNetworkAdapters() {
	perfNETs = gcnew List<PerformanceCounter^>;
	for each(String^ name in SystemAccess::GetNetAdapterNames()) {
		perfNETs->Add(gcnew PerformanceCounter("Network Interface", "Bytes Total/sec", name));
	}
}

SystemAccess::~SystemAccess() {
	perfCPU->Close();
	perfHDD->Close();
	for each(PerformanceCounter^ perf in *perfNETs) {
		perf->Close();
	}
	LOG("Destroyed SystemAccess");
}

float SystemAccess::GetMetric(SystemMetric s) {
	switch(s) {
	case SystemMetric::CPU:
		return getCPUUsage();
	case SystemMetric::RAM:
		return getRAMUsage();
	case SystemMetric::NETWORK:
		return getNETUsage();
	case SystemMetric::HDD:
		return getHDDUsage();
	case SystemMetric::SOUND:
		return getAudioPeak();
	}
	return 0.0f;
}

void SystemAccess::StartESM(SettingsProvider^ p) {
	LOG("ESM will start now");
	/*
	Different Power-save modes:
		-Hibernate
			Indicates a system hibernation power mode. When a system enters hibernation, the contents of its memory are saved to disk before the computer is turned off.
			When the system is restarted, the desktop and previously active programs are restored.
		-Suspend
			Indicates a system suspended power mode. When a system is suspended, the computer switches to a low-power state called standby.
			When a computer is in standby mode, some devices are turned off and the computer uses less power.
			The system can restore itself more quickly than returning from hibernation.
			Because standby does not save the memory state to disk, a power failure while in standby can cause loss of information.
	*/

	if(p->getRawSetting(SettingName::STANDBY_MODE) == "SUSPEND") {
		Application::SetSuspendState(PowerState::Suspend, false, false);
	} else {
		Application::SetSuspendState(PowerState::Hibernate, false, false);
	}
}

List<String^>^ SystemAccess::GetRunningProccesses() {

	List<String^> ^list = gcnew List<String^>();
	array<Process^>^localAll = Process::GetProcesses();

	String^ path;
	for each(Process^ p in localAll) {
		try {
			path = p->MainModule->FileName;
			if(!path->Contains("System32")) {
				list->Add(path);
			}

		} catch(Exception^ e) {
		}
	}

	return list;

}



void SystemAccess::SetPresentationMode(boolean state) {
	if(state) {
		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED); //Application requires screen and system online
	} else {
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED); //reset the requirements
	}
}

void SystemAccess::SetAutoStart(boolean value) {
	using namespace Microsoft::Win32;
	RegistryKey^ rk;
	String^ path = "\"" + Application::ExecutablePath + "\"";
	//LocalMaschine require administrator rights
	rk = Registry::CurrentUser->OpenSubKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
	if(value) {
		rk->SetValue(APP_NAME, path);
		LOG("The autostart has been set");
	} else {
		rk->SetValue(APP_NAME, "");
		LOG("The autostart has been disabled");
	}
}

void SystemAccess::EnableWindowsStandBy() {
	SetThreadExecutionState(ES_CONTINUOUS);
	LOG("Re-Enabled Windows Standby!");
}

void SystemAccess::DisableWindowsStandBy() {
	SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
	LOG("Disabled Windows Standby!");
}

bool SystemAccess::isUserActive() {
	//DWORD ActiveSessionID = WTSGetActiveConsoleSessionId();
	/*

You can call WTSGetActiveConsoleSessionId to get the terminal services (aka "fast user switching" aka "remote desktop") session ID that is currently active on the physical console.

You can call WTSQuerySessionInformation with WTS_CURRENT_SESSION for the session identifier and WTSSessionId for WTSInfoClass to get the terminal services session ID for the current process.

If the active session ID and the current process session ID are the same, the user corresponding to the current process has the active session on the physical console.

If what you want to know is whether the session that the current process is running in is active (but not necessarily on the physical console) you can instead use the WTSConnectState option to WTSQuerySessionInformation.

	*/

	//	DWORD SessionID;
		//WTSQuerySessionInformation(WTS_CURRENT_SESSION, SessionID, WTSSessionId, )
	return true;
}

bool SystemAccess::IsInAutoStart() {
	String^ path = "\"" + Application::ExecutablePath + "\"";
	using namespace Microsoft::Win32;
	RegistryKey^ rk;
	//Local machine require administrator rights
	rk = Registry::CurrentUser->OpenSubKey("Software\\Microsoft\\Windows\\CurrentVersion\\Run", true);
	System::Object^ o = nullptr;
	o = rk->GetValue(APP_NAME, "");
	if(o == nullptr || o->ToString()->Length == 0 || o->ToString() != path) {
		return false;
	} else {
		return true;
	}
}

float SystemAccess::getAudioPeak() {

	using CSCore::CoreAudioAPI::MMDeviceEnumerator;
	using CSCore::CoreAudioAPI::DataFlow;
	using CSCore::CoreAudioAPI::Role;
	using CSCore::CoreAudioAPI::MMDevice;
	using CSCore::CoreAudioAPI::AudioMeterInformation;

	MMDeviceEnumerator ^enumerator = gcnew MMDeviceEnumerator();
	MMDevice ^device = enumerator->GetDefaultAudioEndpoint(DataFlow::Render, Role::Multimedia);

	AudioMeterInformation ^info = AudioMeterInformation::FromDevice(device);

	return info->PeakValue;
}



List<String^>^ SystemAccess::getLoggedInUsers() {
	List<String^>^ loggedInUsers = gcnew List<String^>();

	TerminalServicesManager ^tsmanager = gcnew TerminalServicesManager();
	ITerminalServer ^server = tsmanager->GetLocalServer();

	for each(ITerminalServicesSession^ session in server->GetSessions()) {

		String^ name = session->ClientName;
		if(name != "") {
			loggedInUsers->Add(name);
		}
	}

	LOG("Found total amount of {" + loggedInUsers->Count + "} Users:");
	for each(String^ username in loggedInUsers) {
		if(username != "") {
			LOG("\t" + username);
		}
	}

	return loggedInUsers;
}

String ^ SystemAccess::getActiveUser() {
	return Environment::UserName;
}

String ^ SystemAccess::getStartUserOfStandbye() {
	String^ ThreadStartedUser = Environment::UserName;
	LOG("The User: {" + ThreadStartedUser + "} started Standbye!");
	return ThreadStartedUser;
}

bool SystemAccess::MultiUsersPreventStandby() {
	//Needs Wtsapi32.h
	String^ ThreadStartedUser = getStartUserOfStandbye();
	List<String^>^ loggedInUsers = getLoggedInUsers();
	String^ currentActiveUser = getActiveUser();

	//Ready
	//////////////////////////////////////////////////////////////////////////
	/*
	Cases:
		Current User	Amount & List	Standby
		A				1(A)			yes
		-				1(A)			yes
		B				2(A,B)			no
		-				2(A,B)			no (?)

	*/
	//////////////////////////////////////////////////////////////////////////

	if(loggedInUsers->Count == 1) {
		//Only one user is active! --> The standby can be activated
		LOG("Only one user is active! StandBye can proceed!");
		return false;
	} else if(currentActiveUser == ThreadStartedUser) {
		//There are multiple users logged in, but the active user is the one, who started the process
		LOG("There are multiple users active, but StandBye user is active!");
		return false;
	} else {
		//There are multiple users active and no one is active or an other one is active
		LOG("There are multiple users logged in with a configuration that cancels standby");
		return true;
	}
}

String^ SystemAccess::getStandByeFolderPath() {
	using namespace System::IO;
	if(!SystemAccess::isPortable()) {
		//Gets the AppData Path
		String^ folder = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
		System::IO::Directory::CreateDirectory(folder);
		folder = Path::Combine(folder, "StandBye");
		//Ensures that the folder exists

		return folder;
	} else {
		String^ folder = Directory::GetCurrentDirectory();
		try {
			// Attempt to get a list of security permissions from the folder.
			// This will raise an exception if the path is read only or do not have access to view the permissions.
			Directory::GetAccessControl(folder);

			//is authorized
			return folder;
		} catch(UnauthorizedAccessException^) {
			//Not authorized
			LOG("Not authorized to write to folder!");
			return folder;
		}
	}
}

System::Drawing::Bitmap ^ SystemAccess::getIconOfProcess(String^ path) {

#undef ExtractAssociatedIcon;

	Icon ^ic = Icon::ExtractAssociatedIcon(path);

	return ic->ToBitmap();
	
}

bool SystemAccess::isPortable() {
	String^ line = Environment::CommandLine;
	List<String^>^ commands = gcnew List<String^>(line->Split(' '));
	for each(String^ c in commands) {
		String^ cleaned = c->Trim()->Replace("-", "")->Replace("/", "");
		if(cleaned == "PORTABLE") {
			return true;
		}
	}
	return false;
}

bool SystemAccess::inDebugMode() {
	String^ line = Environment::CommandLine;
	List<String^>^ commands = gcnew List<String^>(line->Split(' '));
	for each(String^ c in commands) {
		String^ cleaned = c->Trim()->Replace("-", "")->Replace("/", "");
		if(cleaned == "DEBUG") {
			return true;
		}
	}
	return false;
}

List<String^> ^SystemAccess::GetNetAdapterNames() {
	System::String ^filter = "MS TCP Loopback interface";
	List<String^> ^nics = gcnew List<String^>();
	PerformanceCounterCategory^ category = gcnew PerformanceCounterCategory("Network Interface");
	LOG("Get Network Adapter Names:");
	if(category->GetInstanceNames() != __nullptr) {
		for each (System::String ^nic in category->GetInstanceNames()) {
			if(!nic->Equals(filter)) {
				nics->Add(nic);
				LOG("\t" + nic);
			}
		}
	}
	return nics;
}

float SystemAccess::GetLastInputTime() {
	//add user32.lib To Dependencies!!
	//int systemUptime = Environment::TickCount; //in millis
	int systemUptime = GetTickCount();
	float idleTicks = 0;
	LASTINPUTINFO lastInputInfo;
	lastInputInfo.cbSize = (UInt32)sizeof(lastInputInfo);
	lastInputInfo.dwTime = 0;

	if(SystemAccess::GetLastInputInfo(&lastInputInfo)) {
		float lastInputTicks = (float)lastInputInfo.dwTime;
		idleTicks = systemUptime - lastInputTicks;
	}
	return idleTicks;

}