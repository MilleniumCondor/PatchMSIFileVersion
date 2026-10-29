#pragma once
#include <Windows.h>
#include <MsiQuery.h>
#include <string>
#include <sstream>
#include <map>
#include <exception>
#include <list>

#pragma comment(lib, "msi.lib")

#define OUTPUT_BUFFER_SIZE	1024

// Unicode uses std::wstring, multibyte uses std::string
#ifdef UNICODE
#define STRING std::wstring
#else
#define STRING std::string
#endif

class MsiFileInfo {

public:
	std::map<STRING, STRING> properties;

	/*
		Retrieve MSI file information
	*/
	MsiFileInfo(STRING path) {
		MSIHANDLE handle = 0;
		UINT hres = 0;

		// Open the MSI file's database
		hres = MsiOpenDatabase(path.c_str(), MSIDBOPEN_READONLY, &handle);
		if (hres != 0) throw_win32("MsiOpenDatabase", hres);
		addHandle(handle);

		// Read the MSI file's property table
		readProperties(handle);

		closeHandle(handle);
	}

	~MsiFileInfo() {
		for (auto handle : handles) {
			closeHandle(handle);
		}
	}



private:
	/*
		Read all properties from a MSI database property table
	*/
	void readProperties(MSIHANDLE handle) {
		MSIHANDLE viewHandle = 0;
		UINT hres = 0;

		// Prepare SQL like view
		hres = MsiDatabaseOpenView(handle, TEXT("SELECT Property, Value FROM Property"), &viewHandle);
		if (hres != 0) throw_win32("MsiDatabaseOpenView", hres);
		addHandle(viewHandle);

		// Execute view's query
		hres = MsiViewExecute(viewHandle, NULL);
		if (hres != 0) throw_win32("MsiViewExecute", hres);

		// Loop through the view's query results
		MSIHANDLE recordHandle = 0;
		while (!(hres = MsiViewFetch(viewHandle, &recordHandle))) {
			addHandle(recordHandle);
			STRING key = getRecordData(recordHandle, 1);
			STRING value = getRecordData(recordHandle, 2);
			properties[key] = value;
			closeHandle(recordHandle);
		}
		if (hres && hres != ERROR_NO_MORE_ITEMS) throw_win32("MsiViewFetch", hres);

		// Close the view's handle
		closeHandle(viewHandle);
	}

	/*
		Read a record and return it's value as string
	 */
	STRING getRecordData(MSIHANDLE recordHandle, UINT field) {
		STRING output(OUTPUT_BUFFER_SIZE, 0);
		DWORD outputLength = OUTPUT_BUFFER_SIZE;
		UINT hres = MsiRecordGetString(recordHandle, field, &output[0], &outputLength);

		if (hres && hres != ERROR_MORE_DATA) throw_win32("MsiRecordGetString", hres);

		output.resize(outputLength);
		return output;
	}

	/*
		Throws a win32 message in a nice format, giving it's error code and message
	*/
	void throw_win32(std::string fnName, UINT hres) {
		std::stringstream sstream;
		sstream << "Exception on " << fnName << " [0x" << std::hex << hres << "]: " << std::system_category().message(hres);
		throw std::exception(sstream.str().c_str());
	}

	/*********** Handle stuff (makes sure that everything is destroyed) ***********/
	std::list<MSIHANDLE> handles;

	void addHandle(MSIHANDLE handle) {
		handles.push_back(handle);
	}

	void closeHandle(MSIHANDLE handle) {
		MsiCloseHandle(handle);
		handles.remove(handle);
	}
};