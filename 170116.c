int VDir::SetCurrentDirectoryA(char *lpBuffer)
{
    char *pPtr;
    int length, nRet = -1;

    pPtr = MapPathA(lpBuffer);
    length = strlen(pPtr);
    if(length > 3 && IsPathSep(pPtr[length-1])) {
	/* don't remove the trailing slash from 'x:\'  */
	pPtr[length-1] = '\0';
    }

    DWORD r = GetFileAttributesA(pPtr);
    if ((r != 0xffffffff) && (r & FILE_ATTRIBUTE_DIRECTORY))
    {
	char szBuffer[(MAX_PATH+1)*2];
	DoGetFullPathNameA(pPtr, sizeof(szBuffer), szBuffer);
	SetDefaultDirA(szBuffer, DriveIndex(szBuffer[0]));
	nRet = 0;
    }

    return nRet;
}