    inline void SetDefaultDirA(char const *pPath, int index)
    {
	SetDirA(pPath, index);
	nDefault = index;
    };