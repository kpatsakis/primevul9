void VDir::SetDefaultA(char const *pDefault)
{
    char szBuffer[MAX_PATH+1];
    char *pPtr;

    if (GetFullPathNameA(pDefault, sizeof(szBuffer), szBuffer, &pPtr)) {
        if (*pDefault != '.' && pPtr != NULL)
	    *pPtr = '\0';

	SetDefaultDirA(szBuffer, DriveIndex(szBuffer[0]));
    }
}