int VDir::SetDirW(WCHAR const *pPath, int index)
{
    WCHAR chr, *ptr;
    int length = 0;
    if (index < driveCount && pPath != NULL) {
	length = wcslen(pPath);
	pMem->Free(dirTableW[index]);
	ptr = dirTableW[index] = (WCHAR*)pMem->Malloc((length+2)*2);
	if (ptr != NULL) {
            char *ansi;
	    wcscpy(ptr, pPath);
	    ptr += length-1;
	    chr = *ptr++;
	    if (chr != '\\' && chr != '/') {
		*ptr++ = '\\';
		*ptr = '\0';
	    }
            ansi = win32_ansipath(dirTableW[index]);
	    length = strlen(ansi);
	    pMem->Free(dirTableA[index]);
	    dirTableA[index] = (char*)pMem->Malloc(length+1);
	    if (dirTableA[index] != NULL) {
		strcpy(dirTableA[index], ansi);
	    }
            win32_free(ansi);
	}
    }

    if(bManageDirectory)
	::SetCurrentDirectoryW(pPath);

    return length;
}