inline void DoGetFullPathNameW(WCHAR* lpBuffer, DWORD dwSize, WCHAR* Dest)
{
    WCHAR *pPtr;

    /*
     * On WinNT GetFullPathName does not fail, (or at least always
     * succeeds when the drive is valid) WinNT does set *Dest to NULL
     * On Win98 GetFullPathName will set last error if it fails, but
     * does not touch *Dest
     */
    *Dest = '\0';
    GetFullPathNameW(lpBuffer, dwSize, Dest, &pPtr);
}