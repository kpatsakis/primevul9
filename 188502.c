static char *ga_get_win_product_name(Error **errp)
{
    HKEY key = NULL;
    DWORD size = 128;
    char *result = g_malloc0(size);
    LONG err = ERROR_SUCCESS;

    err = RegOpenKeyA(HKEY_LOCAL_MACHINE,
                      "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                      &key);
    if (err != ERROR_SUCCESS) {
        error_setg_win32(errp, err, "failed to open registry key");
        goto fail;
    }

    err = RegQueryValueExA(key, "ProductName", NULL, NULL,
                            (LPBYTE)result, &size);
    if (err == ERROR_MORE_DATA) {
        slog("ProductName longer than expected (%lu bytes), retrying",
                size);
        g_free(result);
        result = NULL;
        if (size > 0) {
            result = g_malloc0(size);
            err = RegQueryValueExA(key, "ProductName", NULL, NULL,
                                    (LPBYTE)result, &size);
        }
    }
    if (err != ERROR_SUCCESS) {
        error_setg_win32(errp, err, "failed to retrive ProductName");
        goto fail;
    }

    return result;

fail:
    g_free(result);
    return NULL;
}