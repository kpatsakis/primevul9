void qmp_guest_file_flush(int64_t handle, Error **errp)
{
    HANDLE fh;
    GuestFileHandle *gfh = guest_file_handle_find(handle, errp);
    if (!gfh) {
        return;
    }

    fh = gfh->fh;
    if (!FlushFileBuffers(fh)) {
        error_setg_win32(errp, GetLastError(), "failed to flush file");
    }
}