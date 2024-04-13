    inline char* GetCurrentDirectoryA(int dwBufSize, char *lpBuffer)
    {
	char* ptr = dirTableA[nDefault];
	while (--dwBufSize)
	{
	    if ((*lpBuffer++ = *ptr++) == '\0')
		break;
	}
        *lpBuffer = '\0';
	return /* unused */ NULL;
    };