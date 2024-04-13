void VDir::SetDefaultW(WCHAR const *pDefault)
{
    WCHAR szBuffer[MAX_PATH+1];
    WCHAR *pPtr;

    if (GetFullPathNameW(pDefault, (sizeof(szBuffer)/sizeof(WCHAR)), szBuffer, &pPtr)) {
        if (*pDefault != '.' && pPtr != NULL)
	    *pPtr = '\0';

	SetDefaultDirW(szBuffer, DriveIndex((char)szBuffer[0]));
    }
}