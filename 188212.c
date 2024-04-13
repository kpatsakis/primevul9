
char* php_get_windows_name()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;
	char *major = NULL, *sub = NULL, *retval;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi))) {
		return NULL;
	}

	pGNSI = (PGNSI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if(NULL != pGNSI) {
		pGNSI(&si);
	} else {
		GetSystemInfo(&si);
	}

	if (VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 ) {
		if (osvi.dwMajorVersion == 6) {
			if( osvi.dwMinorVersion == 0 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION ) {
					major = "Windows Vista";
				} else {
					major = "Windows Server 2008";
				}
			} else
			if ( osvi.dwMinorVersion == 1 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION )  {
					major = "Windows 7";
				} else {
					major = "Windows Server 2008 R2";
				}
			} else if ( osvi.dwMinorVersion == 2 ) {
				if( osvi.wProductType == VER_NT_WORKSTATION )  {
					major = "Windows 8";
				} else {
					major = "Windows Server 2012";
				}
			} else {
				major = "Unknown Windows version";
			}

			pGPI = (PGPI) GetProcAddress(GetModuleHandle("kernel32.dll"), "GetProductInfo");
			pGPI(6, 0, 0, 0, &dwType);

			switch (dwType) {
				case PRODUCT_ULTIMATE:
					sub = "Ultimate Edition";
					break;
				case PRODUCT_HOME_PREMIUM:
					sub = "Home Premium Edition";
					break;
				case PRODUCT_HOME_BASIC:
					sub = "Home Basic Edition";
					break;
				case PRODUCT_ENTERPRISE:
					sub = "Enterprise Edition";
					break;
				case PRODUCT_BUSINESS:
					sub = "Business Edition";
					break;
				case PRODUCT_STARTER:
					sub = "Starter Edition";
					break;
				case PRODUCT_CLUSTER_SERVER:
					sub = "Cluster Server Edition";
					break;
				case PRODUCT_DATACENTER_SERVER:
					sub = "Datacenter Edition";
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					sub = "Datacenter Edition (core installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER:
					sub = "Enterprise Edition";
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					sub = "Enterprise Edition (core installation)";
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					sub = "Enterprise Edition for Itanium-based Systems";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER:
					sub = "Small Business Server";
					break;
				case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
					sub = "Small Business Server Premium Edition";
					break;
				case PRODUCT_STANDARD_SERVER:
					sub = "Standard Edition";
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					sub = "Standard Edition (core installation)";
					break;
				case PRODUCT_WEB_SERVER:
					sub = "Web Server Edition";
					break;
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 ) {
			if (GetSystemMetrics(SM_SERVERR2))
				major = "Windows Server 2003 R2";
			else if (osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER)
				major = "Windows Storage Server 2003";
			else if (osvi.wSuiteMask==VER_SUITE_WH_SERVER)
				major = "Windows Home Server";
			else if (osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) {
				major = "Windows XP Professional x64 Edition";
			} else {
				major = "Windows Server 2003";
			}

			/* Test for the server type. */
			if ( osvi.wProductType != VER_NT_WORKSTATION ) {
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 ) {
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						sub = "Datacenter Edition for Itanium-based Systems";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sub = "Enterprise Edition for Itanium-based Systems";
				}

				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 ) {
					if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						sub = "Datacenter x64 Edition";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sub = "Enterprise x64 Edition";
					else sub = "Standard x64 Edition";
				} else {
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
						sub = "Compute Cluster Edition";
					else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
						sub = "Datacenter Edition";
					else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						sub = "Enterprise Edition";
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
						sub = "Web Edition";
					else sub = "Standard Edition";
				}
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )	{
			major = "Windows XP";
			if( osvi.wSuiteMask & VER_SUITE_PERSONAL ) {
				sub = "Home Edition";
			} else if (GetSystemMetrics(SM_MEDIACENTER)) {
				sub = "Media Center Edition";
			} else if (GetSystemMetrics(SM_STARTER)) {
				sub = "Starter Edition";
			} else if (GetSystemMetrics(SM_TABLETPC)) {
				sub = "Tablet PC Edition";
			} else {
				sub = "Professional";
			}
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 ) {
			major = "Windows 2000";

			if (osvi.wProductType == VER_NT_WORKSTATION ) {
				sub = "Professional";
			} else {
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					sub = "Datacenter Server";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					sub = "Advanced Server";
				else sub = "Server";
			}
		}
	} else {
		return NULL;
	}

	spprintf(&retval, 0, "%s%s%s%s%s", major, sub?" ":"", sub?sub:"", osvi.szCSDVersion[0] != '\0'?" ":"", osvi.szCSDVersion);
	return retval;