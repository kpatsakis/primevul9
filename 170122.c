    inline WCHAR* GetCurrentDirectoryW(int dwBufSize, WCHAR *lpBuffer)
    {
	WCHAR* ptr = dirTableW[nDefault];
	while (--dwBufSize)
	{
	    if ((*lpBuffer++ = *ptr++) == '\0')
		break;
	}
        *lpBuffer = '\0';
	return /* unused */ NULL;
    };