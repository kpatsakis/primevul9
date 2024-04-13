int64_t qmp_guest_file_open(const char *path, bool has_mode,
                            const char *mode, Error **errp)
{
    int64_t fd;
    HANDLE fh;
    HANDLE templ_file = NULL;
    DWORD share_mode = FILE_SHARE_READ;
    DWORD flags_and_attr = FILE_ATTRIBUTE_NORMAL;
    LPSECURITY_ATTRIBUTES sa_attr = NULL;
    OpenFlags *guest_flags;

    if (!has_mode) {
        mode = "r";
    }
    slog("guest-file-open called, filepath: %s, mode: %s", path, mode);
    guest_flags = find_open_flag(mode);
    if (guest_flags == NULL) {
        error_setg(errp, "invalid file open mode");
        return -1;
    }

    fh = CreateFile(path, guest_flags->desired_access, share_mode, sa_attr,
                    guest_flags->creation_disposition, flags_and_attr,
                    templ_file);
    if (fh == INVALID_HANDLE_VALUE) {
        error_setg_win32(errp, GetLastError(), "failed to open file '%s'",
                         path);
        return -1;
    }

    /* set fd non-blocking to avoid common use cases (like reading from a
     * named pipe) from hanging the agent
     */
    handle_set_nonblocking(fh);

    fd = guest_file_handle_add(fh, errp);
    if (fd < 0) {
        CloseHandle(fh);
        error_setg(errp, "failed to add handle to qmp handle table");
        return -1;
    }

    slog("guest-file-open, handle: % " PRId64, fd);
    return fd;
}