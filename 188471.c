static char *ga_get_win_name(OSVERSIONINFOEXW const *os_version, bool id)
{
    DWORD major = os_version->dwMajorVersion;
    DWORD minor = os_version->dwMinorVersion;
    int tbl_idx = (os_version->wProductType != VER_NT_WORKSTATION);
    ga_matrix_lookup_t const *table = WIN_VERSION_MATRIX[tbl_idx];
    while (table->version != NULL) {
        if (major == table->major && minor == table->minor) {
            if (id) {
                return g_strdup(table->version_id);
            } else {
                return g_strdup(table->version);
            }
        }
        ++table;
    }
    slog("failed to lookup Windows version: major=%lu, minor=%lu",
        major, minor);
    return g_strdup("N/A");
}