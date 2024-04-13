    inline const WCHAR *GetDirW(int index)
    {
	WCHAR *ptr = dirTableW[index];
	if (!ptr) {
	    /* simulate the existence of this drive */
	    ptr = szLocalBufferW;
	    ptr[0] = 'A' + index;
	    ptr[1] = ':';
	    ptr[2] = '\\';
	    ptr[3] = 0;
	}
	return ptr;
    };