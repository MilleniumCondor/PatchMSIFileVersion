// PatchMsiFileVersion.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <MsiQuery.h>
#include <filesystem>
#include <string>
#include "PatchMsiFileVersion.h"

#pragma comment(lib, "msi.lib")
#pragma comment(lib, "version.lib")

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
  //std::cout << "Hello World!\n";
	if (argc < 2)
	{
		std::cout << "Usage: PatchMsiFileVersion <path_to_msi_file>\n";
		return 1;
	}

	TCHAR szSource[OUTPUT_BUFFER_SIZE];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[1], -1, szSource, OUTPUT_BUFFER_SIZE);

	STRING szVersionFile(szSource);

	try
	{
		if (!fs::exists(szVersionFile))
		{
			std::wcout << L"File does not exist : " << szVersionFile << L"\n";
			return 1;
		}

		MsiFileInfo msi(szVersionFile);
		STRING productVersion = msi.properties[L"ProductVersion"];

		STRING szNewFilename;
		size_t substring = szVersionFile.find(L"[VERSION]");
		if (substring != STRING::npos)
		{
			szNewFilename = szVersionFile;
			szNewFilename.replace(substring, 9, productVersion);
		}

		if (szNewFilename.empty())
		{
			std::cout << "Patch error or no [VERSION] placeholder found\n";
			return 1;
		}
		if (!fs::copy_file(szVersionFile, szNewFilename, fs::copy_options::overwrite_existing))  	return 1;
		fs::remove(szVersionFile);

		std::wcout << L"Patched MSI file version: " << productVersion << L"\n";
	}
	catch (const fs::filesystem_error& e)
	{
		std::cout << "Filesystem error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}

