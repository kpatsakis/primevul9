int64_t qmp_guest_file_open(const char *path, bool has_mode, const char *mode,
                            Error **errp)
{
    FILE *fh;
    Error *local_err = NULL;
    int64_t handle;

    if (!has_mode) {
        mode = "r";
    }
    slog("guest-file-open called, filepath: %s, mode: %s", path, mode);
    fh = safe_open_or_create(path, mode, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return -1;
    }

    /* set fd non-blocking to avoid common use cases (like reading from a
     * named pipe) from hanging the agent
     */
    qemu_set_nonblock(fileno(fh));

    handle = guest_file_handle_add(fh, errp);
    if (handle < 0) {
        fclose(fh);
        return -1;
    }

    slog("guest-file-open, handle: %" PRId64, handle);
    return handle;
}