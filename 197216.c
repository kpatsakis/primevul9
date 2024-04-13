static BOOL shadow_server_init_certificate(rdpShadowServer* server)
{
	char* filepath;
	MAKECERT_CONTEXT* makecert = NULL;
	BOOL ret = FALSE;
	const char* makecert_argv[6] = { "makecert", "-rdp", "-live", "-silent", "-y", "5" };
	int makecert_argc = (sizeof(makecert_argv) / sizeof(char*));

	if (!PathFileExistsA(server->ConfigPath) && !PathMakePathA(server->ConfigPath, 0))
	{
		WLog_ERR(TAG, "Failed to create directory '%s'", server->ConfigPath);
		return FALSE;
	}

	if (!(filepath = GetCombinedPath(server->ConfigPath, "shadow")))
		return FALSE;

	if (!PathFileExistsA(filepath) && !PathMakePathA(filepath, 0))
	{
		if (!CreateDirectoryA(filepath, 0))
		{
			WLog_ERR(TAG, "Failed to create directory '%s'", filepath);
			goto out_fail;
		}
	}

	server->CertificateFile = GetCombinedPath(filepath, "shadow.crt");
	server->PrivateKeyFile = GetCombinedPath(filepath, "shadow.key");

	if (!server->CertificateFile || !server->PrivateKeyFile)
		goto out_fail;

	if ((!PathFileExistsA(server->CertificateFile)) || (!PathFileExistsA(server->PrivateKeyFile)))
	{
		makecert = makecert_context_new();

		if (!makecert)
			goto out_fail;

		if (makecert_context_process(makecert, makecert_argc, (char**)makecert_argv) < 0)
			goto out_fail;

		if (makecert_context_set_output_file_name(makecert, "shadow") != 1)
			goto out_fail;

		if (!PathFileExistsA(server->CertificateFile))
		{
			if (makecert_context_output_certificate_file(makecert, filepath) != 1)
				goto out_fail;
		}

		if (!PathFileExistsA(server->PrivateKeyFile))
		{
			if (makecert_context_output_private_key_file(makecert, filepath) != 1)
				goto out_fail;
		}
	}

	ret = TRUE;
out_fail:
	makecert_context_free(makecert);
	free(filepath);
	return ret;
}