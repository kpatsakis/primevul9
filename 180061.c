ves_icall_System_Environment_GetWindowsFolderPath (int folder)
{
#if defined (PLATFORM_WIN32)
	#ifndef CSIDL_FLAG_CREATE
		#define CSIDL_FLAG_CREATE	0x8000
	#endif

	WCHAR path [MAX_PATH];
	/* Create directory if no existing */
	if (SUCCEEDED (SHGetFolderPathW (NULL, folder | CSIDL_FLAG_CREATE, NULL, 0, path))) {
		int len = 0;
		while (path [len])
			++ len;
		return mono_string_new_utf16 (mono_domain_get (), path, len);
	}
#else
	g_warning ("ves_icall_System_Environment_GetWindowsFolderPath should only be called on Windows!");
#endif
	return mono_string_new (mono_domain_get (), "");
}