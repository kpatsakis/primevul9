DWORD VDir::CalculateEnvironmentSpace(void)
{   /* the current directory environment strings are stored as '=D:=d:\path' */
    int index;
    DWORD dwSize = 0;
    for (index = 0; index < driveCount; ++index) {
	if (dirTableA[index] != NULL) {
	    dwSize += strlen(dirTableA[index]) + 5;  /* add 1 for trailing NULL and 4 for '=D:=' */
	}
    }
    return dwSize;
}