void VDir::FromEnvA(char *pEnv, int index)
{   /* gets the directory for index from the environment variable. */
    while (*pEnv != '\0') {
	if ((pEnv[0] == '=') && (DriveIndex(pEnv[1]) == index)) {
	    SetDirA(&pEnv[4], index);
	    break;
	}
	else
	    pEnv += strlen(pEnv)+1;
    }
}