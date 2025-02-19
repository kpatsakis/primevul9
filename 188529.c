struct GuestFileRead *qmp_guest_file_read(int64_t handle, bool has_count,
                                          int64_t count, Error **errp)
{
    GuestFileHandle *gfh = guest_file_handle_find(handle, errp);
    GuestFileRead *read_data = NULL;
    guchar *buf;
    FILE *fh;
    size_t read_count;

    if (!gfh) {
        return NULL;
    }

    if (!has_count) {
        count = QGA_READ_COUNT_DEFAULT;
    } else if (count < 0 || count >= UINT32_MAX) {
        error_setg(errp, "value '%" PRId64 "' is invalid for argument count",
                   count);
        return NULL;
    }

    fh = gfh->fh;

    /* explicitly flush when switching from writing to reading */
    if (gfh->state == RW_STATE_WRITING) {
        int ret = fflush(fh);
        if (ret == EOF) {
            error_setg_errno(errp, errno, "failed to flush file");
            return NULL;
        }
        gfh->state = RW_STATE_NEW;
    }

    buf = g_malloc0(count+1);
    read_count = fread(buf, 1, count, fh);
    if (ferror(fh)) {
        error_setg_errno(errp, errno, "failed to read file");
        slog("guest-file-read failed, handle: %" PRId64, handle);
    } else {
        buf[read_count] = 0;
        read_data = g_new0(GuestFileRead, 1);
        read_data->count = read_count;
        read_data->eof = feof(fh);
        if (read_count) {
            read_data->buf_b64 = g_base64_encode(buf, read_count);
        }
        gfh->state = RW_STATE_READING;
    }
    g_free(buf);
    clearerr(fh);

    return read_data;
}