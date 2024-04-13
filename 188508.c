void qmp_guest_set_time(bool has_time, int64_t time_ns, Error **errp)
{
    Error *local_err = NULL;
    SYSTEMTIME ts;
    FILETIME tf;
    LONGLONG time;

    if (!has_time) {
        /* Unfortunately, Windows libraries don't provide an easy way to access
         * RTC yet:
         *
         * https://msdn.microsoft.com/en-us/library/aa908981.aspx
         *
         * Instead, a workaround is to use the Windows win32tm command to
         * resync the time using the Windows Time service.
         */
        LPVOID msg_buffer;
        DWORD ret_flags;

        HRESULT hr = system("w32tm /resync /nowait");

        if (GetLastError() != 0) {
            strerror_s((LPTSTR) & msg_buffer, 0, errno);
            error_setg(errp, "system(...) failed: %s", (LPCTSTR)msg_buffer);
        } else if (hr != 0) {
            if (hr == HRESULT_FROM_WIN32(ERROR_SERVICE_NOT_ACTIVE)) {
                error_setg(errp, "Windows Time service not running on the "
                                 "guest");
            } else {
                if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                   FORMAT_MESSAGE_FROM_SYSTEM |
                                   FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                   (DWORD)hr, MAKELANGID(LANG_NEUTRAL,
                                   SUBLANG_DEFAULT), (LPTSTR) & msg_buffer, 0,
                                   NULL)) {
                    error_setg(errp, "w32tm failed with error (0x%lx), couldn'"
                                     "t retrieve error message", hr);
                } else {
                    error_setg(errp, "w32tm failed with error (0x%lx): %s", hr,
                               (LPCTSTR)msg_buffer);
                    LocalFree(msg_buffer);
                }
            }
        } else if (!InternetGetConnectedState(&ret_flags, 0)) {
            error_setg(errp, "No internet connection on guest, sync not "
                             "accurate");
        }
        return;
    }

    /* Validate time passed by user. */
    if (time_ns < 0 || time_ns / 100 > INT64_MAX - W32_FT_OFFSET) {
        error_setg(errp, "Time %" PRId64 "is invalid", time_ns);
        return;
    }

    time = time_ns / 100 + W32_FT_OFFSET;

    tf.dwLowDateTime = (DWORD) time;
    tf.dwHighDateTime = (DWORD) (time >> 32);

    if (!FileTimeToSystemTime(&tf, &ts)) {
        error_setg(errp, "Failed to convert system time %d",
                   (int)GetLastError());
        return;
    }

    acquire_privilege(SE_SYSTEMTIME_NAME, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    if (!SetSystemTime(&ts)) {
        error_setg(errp, "Failed to set time to guest: %d", (int)GetLastError());
        return;
    }
}