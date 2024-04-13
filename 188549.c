int64_t qmp_guest_get_time(Error **errp)
{
    SYSTEMTIME ts = {0};
    FILETIME tf;

    GetSystemTime(&ts);
    if (ts.wYear < 1601 || ts.wYear > 30827) {
        error_setg(errp, "Failed to get time");
        return -1;
    }

    if (!SystemTimeToFileTime(&ts, &tf)) {
        error_setg(errp, "Failed to convert system time: %d", (int)GetLastError());
        return -1;
    }

    return ((((int64_t)tf.dwHighDateTime << 32) | tf.dwLowDateTime)
                - W32_FT_OFFSET) * 100;
}