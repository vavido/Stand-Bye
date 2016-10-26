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

using System::String;

ref class ProcessItem : System::Windows::Forms::ListViewItem {
	String^ path;
	Bitmap^ icon;
public:
	~ProcessItem();
	ProcessItem(String^ settings_value, System::Windows::Forms::ListView^ list);
	void Write(SettingsProvider^ settings_provider);
	String^ GetPath();
	void addIconToLists(System::Windows::Forms::ListView^ list);
};
