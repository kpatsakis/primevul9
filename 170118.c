inline void DoGetFullPathNameA(char* lpBuffer, DWORD dwSize, char* Dest)
{
    char *pPtr;

    /*
     * On WinNT GetFullPathName does not fail, (or at least always
     * succeeds when the drive is valid) WinNT does set *Dest to NULL
     * On Win98 GetFullPathName will set last error if it fails, but
     * does not touch *Dest
     */
    *Dest = '\0';
    GetFullPathNameA(lpBuffer, dwSize, Dest, &pPtr);
}