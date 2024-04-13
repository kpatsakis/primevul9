static int shadow_server_init_config_path(rdpShadowServer* server)
{
#ifdef _WIN32

	if (!server->ConfigPath)
	{
		server->ConfigPath = GetEnvironmentSubPath("LOCALAPPDATA", "freerdp");
	}

#endif
#ifdef __APPLE__

	if (!server->ConfigPath)
	{
		char* userLibraryPath;
		char* userApplicationSupportPath;
		userLibraryPath = GetKnownSubPath(KNOWN_PATH_HOME, "Library");

		if (userLibraryPath)
		{
			if (!PathFileExistsA(userLibraryPath) && !PathMakePathA(userLibraryPath, 0))
			{
				WLog_ERR(TAG, "Failed to create directory '%s'", userLibraryPath);
				free(userLibraryPath);
				return -1;
			}

			userApplicationSupportPath = GetCombinedPath(userLibraryPath, "Application Support");

			if (userApplicationSupportPath)
			{
				if (!PathFileExistsA(userApplicationSupportPath) &&
				    !PathMakePathA(userApplicationSupportPath, 0))
				{
					WLog_ERR(TAG, "Failed to create directory '%s'", userApplicationSupportPath);
					free(userLibraryPath);
					free(userApplicationSupportPath);
					return -1;
				}

				server->ConfigPath = GetCombinedPath(userApplicationSupportPath, "freerdp");
			}

			free(userLibraryPath);
			free(userApplicationSupportPath);
		}
	}

#endif

	if (!server->ConfigPath)
	{
		char* configHome;
		configHome = GetKnownPath(KNOWN_PATH_XDG_CONFIG_HOME);

		if (configHome)
		{
			if (!PathFileExistsA(configHome) && !PathMakePathA(configHome, 0))
			{
				WLog_ERR(TAG, "Failed to create directory '%s'", configHome);
				free(configHome);
				return -1;
			}

			server->ConfigPath = GetKnownSubPath(KNOWN_PATH_XDG_CONFIG_HOME, "freerdp");
			free(configHome);
		}
	}

	if (!server->ConfigPath)
		return -1; /* no usable config path */

	return 1;
}