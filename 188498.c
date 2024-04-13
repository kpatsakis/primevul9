void qmp_guest_file_close(int64_t handle, Error **errp)
{
    bool ret;
    GuestFileHandle *gfh = guest_file_handle_find(handle, errp);
    slog("guest-file-close called, handle: %" PRId64, handle);
    if (gfh == NULL) {
        return;
    }
    ret = CloseHandle(gfh->fh);
    if (!ret) {
        error_setg_win32(errp, GetLastError(), "failed close handle");
        return;
    }

    QTAILQ_REMOVE(&guest_file_state.filehandles, gfh, next);
    g_free(gfh);
}