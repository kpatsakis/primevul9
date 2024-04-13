e_util_win32_initialize (void)
{
	gchar module_filename[2048 + 1];
	DWORD chars;

	/* Reduce risks */
	{
		typedef BOOL (WINAPI *t_SetDllDirectoryA) (LPCSTR lpPathName);
		t_SetDllDirectoryA p_SetDllDirectoryA;

		p_SetDllDirectoryA = GetProcAddress (
			GetModuleHandle ("kernel32.dll"),
			"SetDllDirectoryA");

		if (p_SetDllDirectoryA != NULL)
			p_SetDllDirectoryA ("");
	}
#ifndef _WIN64
	{
		typedef BOOL (WINAPI *t_SetProcessDEPPolicy) (DWORD dwFlags);
		t_SetProcessDEPPolicy p_SetProcessDEPPolicy;

		p_SetProcessDEPPolicy = GetProcAddress (
			GetModuleHandle ("kernel32.dll"),
			"SetProcessDEPPolicy");

		if (p_SetProcessDEPPolicy != NULL)
			p_SetProcessDEPPolicy (
				PROCESS_DEP_ENABLE |
				PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);
	}
#endif

	if (fileno (stdout) != -1 && _get_osfhandle (fileno (stdout)) != -1) {
		/* stdout is fine, presumably redirected to a file or pipe */
	} else {
		typedef BOOL (* WINAPI AttachConsole_t) (DWORD);

		AttachConsole_t p_AttachConsole =
			(AttachConsole_t) GetProcAddress (
			GetModuleHandle ("kernel32.dll"), "AttachConsole");

		if (p_AttachConsole && p_AttachConsole (ATTACH_PARENT_PROCESS)) {
			freopen ("CONOUT$", "w", stdout);
			dup2 (fileno (stdout), 1);
			freopen ("CONOUT$", "w", stderr);
			dup2 (fileno (stderr), 2);
		}
	}

	chars = GetModuleFileNameA (hmodule, module_filename, 2048);
	if (chars > 0) {
		gchar *path;

		module_filename[chars] = '\0';

		path = strrchr (module_filename, '\\');
		if (path)
			path[1] = '\0';

		path = g_build_path (";", module_filename, g_getenv ("PATH"), NULL);

		if (!g_setenv ("PATH", path, TRUE))
			g_warning ("Could not set PATH for Evolution and its child processes");

		g_free (path);
	}

	/* Make sure D-Bus is running. The executable makes sure the daemon
	   is not restarted, thus it's safe to be called witn D-Bus already
	   running. */
	if (system ("dbus-launch.exe") != 0) {
		/* Ignore, just to mute compiler warning */;
	}
}