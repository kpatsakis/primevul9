    inline int DriveIndex(char chr)
    {
	if (chr == '\\' || chr == '/')
	    return ('Z'-'A')+1;
	return (chr | 0x20)-'a';
    };