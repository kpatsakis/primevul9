int VDir::SetDirA(char const *pPath, int index)
{
    char chr, *ptr;
    int length = 0;
    WCHAR wBuffer[MAX_PATH+1];
    if (index < driveCount && pPath != NULL) {
	length = strlen(pPath);
	pMem->Free(dirTableA[index]);
	ptr = dirTableA[index] = (char*)pMem->Malloc(length+2);
	if (ptr != NULL) {
	    strcpy(ptr, pPath);
	    ptr += length-1;
	    chr = *ptr++;
	    if (chr != '\\' && chr != '/') {
		*ptr++ = '\\';
		*ptr = '\0';
	    }
	    MultiByteToWideChar(CP_ACP, 0, dirTableA[index], -1,
		    wBuffer, (sizeof(wBuffer)/sizeof(WCHAR)));
	    length = wcslen(wBuffer);
	    pMem->Free(dirTableW[index]);
	    dirTableW[index] = (WCHAR*)pMem->Malloc((length+1)*2);
	    if (dirTableW[index] != NULL) {
		wcscpy(dirTableW[index], wBuffer);
	    }
	}
    }

    if(bManageDirectory)
	::SetCurrentDirectoryA(pPath);

    return length;
}