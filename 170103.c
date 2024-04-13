LPSTR VDir::BuildEnvironmentSpace(LPSTR lpStr)
{   /* store the current directory environment strings as '=D:=d:\path' */
    int index, length;
    LPSTR lpDirStr;
    for (index = 0; index < driveCount; ++index) {
	lpDirStr = dirTableA[index];
	if (lpDirStr != NULL) {
	    lpStr[0] = '=';
	    lpStr[1] = lpDirStr[0];
	    lpStr[2] = '\0';
	    CharUpper(&lpStr[1]);
	    lpStr[2] = ':';
	    lpStr[3] = '=';
	    strcpy(&lpStr[4], lpDirStr);
	    length = strlen(lpDirStr);
	    lpStr += length + 5; /* add 1 for trailing NULL and 4 for '=D:=' */
	    if (length > 3 && IsPathSep(lpStr[-2])) {
		lpStr[-2] = '\0';   /* remove the trailing path separator */
		--lpStr;
	    }
	}
    }
    return lpStr;
}