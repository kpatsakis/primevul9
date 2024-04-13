    inline void SetDefaultDirW(WCHAR const *pPath, int index)
    {
	SetDirW(pPath, index);
	nDefault = index;
    };