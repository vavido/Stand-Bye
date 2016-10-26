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


using System::String;

ref class BasicFunc {
public:


	///<summary>Returns an metro font with specified size in points</summary>
	static System::Drawing::Font^ getMetroFont(float size);

	///<summary>Returns the log file path and ensures that the file is accessible</summary>
	static String^ getLogFilePath();

	///<summary>Logs an specified statement or event.</summary>
	static void Log(String^  text);

	///<summary>Logs an specified statement or event.</summary>
	static void Log(String^ text);

	///<summary>Logs an exception</summary>
	static void Log(System::Exception^ exception);

	///<summary>Opens website in new tab</summary>
	static void openLink(String^ url);

	///<summary>Deletes all log files</summary>
	static void cleanLogFiles();
};
