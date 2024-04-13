static DWORD get_interface_index(const char *guid)
{
    ULONG index;
    DWORD status;
    wchar_t wbuf[INTERFACE_PATH_BUF_SZ];
    snwprintf(wbuf, INTERFACE_PATH_BUF_SZ, L"\\device\\tcpip_%s", guid);
    wbuf[INTERFACE_PATH_BUF_SZ - 1] = 0;
    status = GetAdapterIndex (wbuf, &index);
    if (status != NO_ERROR) {
        return (DWORD)~0;
    } else {
        return index;
    }
}