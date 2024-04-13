int git_path_validate_system_file_ownership(const char *path)
{
#ifndef GIT_WIN32
	GIT_UNUSED(path);
	return GIT_OK;
#else
	git_win32_path buf;
	PSID owner_sid;
	PSECURITY_DESCRIPTOR descriptor = NULL;
	HANDLE token;
	TOKEN_USER *info = NULL;
	DWORD err, len;
	int ret;

	if (git_win32_path_from_utf8(buf, path) < 0)
		return -1;

	err = GetNamedSecurityInfoW(buf, SE_FILE_OBJECT,
				    OWNER_SECURITY_INFORMATION |
					    DACL_SECURITY_INFORMATION,
				    &owner_sid, NULL, NULL, NULL, &descriptor);

	if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
		ret = GIT_ENOTFOUND;
		goto cleanup;
	}

	if (err != ERROR_SUCCESS) {
		git_error_set(GIT_ERROR_OS, "failed to get security information");
		ret = GIT_ERROR;
		goto cleanup;
	}

	if (!IsValidSid(owner_sid)) {
		git_error_set(GIT_ERROR_INVALID, "programdata configuration file owner is unknown");
		ret = GIT_ERROR;
		goto cleanup;
	}

	if (IsWellKnownSid(owner_sid, WinBuiltinAdministratorsSid) ||
	    IsWellKnownSid(owner_sid, WinLocalSystemSid)) {
		ret = GIT_OK;
		goto cleanup;
	}

	/* Obtain current user's SID */
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) &&
	    !GetTokenInformation(token, TokenUser, NULL, 0, &len)) {
		info = git__malloc(len);
		GIT_ERROR_CHECK_ALLOC(info);
		if (!GetTokenInformation(token, TokenUser, info, len, &len)) {
			git__free(info);
			info = NULL;
		}
	}

	/*
	 * If the file is owned by the same account that is running the current
	 * process, it's okay to read from that file.
	 */
	if (info && EqualSid(owner_sid, info->User.Sid))
		ret = GIT_OK;
	else {
		git_error_set(GIT_ERROR_INVALID, "programdata configuration file owner is not valid");
		ret = GIT_ERROR;
	}
	free(info);

cleanup:
	if (descriptor)
		LocalFree(descriptor);

	return ret;
#endif
}