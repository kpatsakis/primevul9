static char *guest_wctomb_dup(WCHAR *wstr)
{
    char *str;
    size_t i;

    i = wcslen(wstr) + 1;
    str = g_malloc(i);
    WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK,
                        wstr, -1, str, i, NULL, NULL);
    return str;
}