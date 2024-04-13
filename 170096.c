void VDir::FromEnvW(WCHAR *pEnv, int index)
{   /* gets the directory for index from the environment variable. */
    while (*pEnv != '\0') {
	if ((pEnv[0] == '=') && (DriveIndex((char)pEnv[1]) == index)) {
	    SetDirW(&pEnv[4], index);
	    break;
	}
	else
	    pEnv += wcslen(pEnv)+1;
    }
}