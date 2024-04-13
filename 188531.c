void qmp_guest_file_close(int64_t handle, Error **errp)
{
    GuestFileHandle *gfh = guest_file_handle_find(handle, errp);
    int ret;

    slog("guest-file-close called, handle: %" PRId64, handle);
    if (!gfh) {
        return;
    }

    ret = fclose(gfh->fh);
    if (ret == EOF) {
        error_setg_errno(errp, errno, "failed to close handle");
        return;
    }

    QTAILQ_REMOVE(&guest_file_state.filehandles, gfh, next);
    g_free(gfh);
}