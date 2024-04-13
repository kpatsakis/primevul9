 */
PHPAPI char *php_get_uname(char mode)
{
	char *php_uname;
	char tmp_uname[256];
#ifdef PHP_WIN32
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	char ComputerName[MAX_COMPUTERNAME_LENGTH + 1];

	GetComputerName(ComputerName, &dwSize);

	if (mode == 's') {
		php_uname = "Windows NT";
	} else if (mode == 'r') {
		snprintf(tmp_uname, sizeof(tmp_uname), "%d.%d", dwWindowsMajorVersion, dwWindowsMinorVersion);
		php_uname = tmp_uname;
	} else if (mode == 'n') {
		php_uname = ComputerName;
	} else if (mode == 'v') {
		char *winver = php_get_windows_name();
		dwBuild = (DWORD)(HIWORD(dwVersion));
		if(winver == NULL) {
			snprintf(tmp_uname, sizeof(tmp_uname), "build %d", dwBuild);
		} else {
			snprintf(tmp_uname, sizeof(tmp_uname), "build %d (%s)", dwBuild, winver);
		}
		php_uname = tmp_uname;
		if(winver) {
			efree(winver);
		}
	} else if (mode == 'm') {
		php_get_windows_cpu(tmp_uname, sizeof(tmp_uname));
		php_uname = tmp_uname;
	} else { /* assume mode == 'a' */
		char *winver = php_get_windows_name();
		char wincpu[20];

		php_get_windows_cpu(wincpu, sizeof(wincpu));
		dwBuild = (DWORD)(HIWORD(dwVersion));
		snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d build %d (%s) %s",
				 "Windows NT", ComputerName,
				 dwWindowsMajorVersion, dwWindowsMinorVersion, dwBuild, winver?winver:"unknown", wincpu);
		if(winver) {
			efree(winver);
		}
		php_uname = tmp_uname;
	}
#else
#ifdef HAVE_SYS_UTSNAME_H
	struct utsname buf;
	if (uname((struct utsname *)&buf) == -1) {
		php_uname = PHP_UNAME;
	} else {
#ifdef NETWARE
		if (mode == 's') {
			php_uname = buf.sysname;
		} else if (mode == 'r') {
			snprintf(tmp_uname, sizeof(tmp_uname), "%d.%d.%d",
					 buf.netware_major, buf.netware_minor, buf.netware_revision);
			php_uname = tmp_uname;
		} else if (mode == 'n') {
			php_uname = buf.servername;
		} else if (mode == 'v') {
			snprintf(tmp_uname, sizeof(tmp_uname), "libc-%d.%d.%d #%d",
					 buf.libmajor, buf.libminor, buf.librevision, buf.libthreshold);
			php_uname = tmp_uname;
		} else if (mode == 'm') {
			php_uname = buf.machine;
		} else { /* assume mode == 'a' */
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %d.%d.%d libc-%d.%d.%d #%d %s",
					 buf.sysname, buf.servername,
					 buf.netware_major, buf.netware_minor, buf.netware_revision,
					 buf.libmajor, buf.libminor, buf.librevision, buf.libthreshold,
					 buf.machine);
			php_uname = tmp_uname;
		}
#else
		if (mode == 's') {
			php_uname = buf.sysname;
		} else if (mode == 'r') {
			php_uname = buf.release;
		} else if (mode == 'n') {
			php_uname = buf.nodename;
		} else if (mode == 'v') {
			php_uname = buf.version;
		} else if (mode == 'm') {
			php_uname = buf.machine;
		} else { /* assume mode == 'a' */
			snprintf(tmp_uname, sizeof(tmp_uname), "%s %s %s %s %s",
					 buf.sysname, buf.nodename, buf.release, buf.version,
					 buf.machine);
			php_uname = tmp_uname;
		}
#endif /* NETWARE */
	}
#else
	php_uname = PHP_UNAME;
#endif
#endif
	return estrdup(php_uname);