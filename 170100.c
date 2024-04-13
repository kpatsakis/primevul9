int VDir::SetCurrentDirectoryW(WCHAR *lpBuffer)
{
    WCHAR *pPtr;
    int length, nRet = -1;

    pPtr = MapPathW(lpBuffer);
    length = wcslen(pPtr);
    if(length > 3 && IsPathSep(pPtr[length-1])) {
	/* don't remove the trailing slash from 'x:\'  */
	pPtr[length-1] = '\0';
    }

    DWORD r = GetFileAttributesW(pPtr);
    if ((r != 0xffffffff) && (r & FILE_ATTRIBUTE_DIRECTORY))
    {
	WCHAR wBuffer[(MAX_PATH+1)*2];
	DoGetFullPathNameW(pPtr, (sizeof(wBuffer)/sizeof(WCHAR)), wBuffer);
	SetDefaultDirW(wBuffer, DriveIndex((char)wBuffer[0]));
	nRet = 0;
    }

    return nRet;
}