struct GuestFileSeek *qmp_guest_file_seek(int64_t handle, int64_t offset,
                                          GuestFileWhence *whence_code,
                                          Error **errp)
{
    GuestFileHandle *gfh = guest_file_handle_find(handle, errp);
    GuestFileSeek *seek_data = NULL;
    FILE *fh;
    int ret;
    int whence;
    Error *err = NULL;

    if (!gfh) {
        return NULL;
    }

    /* We stupidly exposed 'whence':'int' in our qapi */
    whence = ga_parse_whence(whence_code, &err);
    if (err) {
        error_propagate(errp, err);
        return NULL;
    }

    fh = gfh->fh;
    ret = fseek(fh, offset, whence);
    if (ret == -1) {
        error_setg_errno(errp, errno, "failed to seek file");
        if (errno == ESPIPE) {
            /* file is non-seekable, stdio shouldn't be buffering anyways */
            gfh->state = RW_STATE_NEW;
        }
    } else {
        seek_data = g_new0(GuestFileSeek, 1);
        seek_data->position = ftell(fh);
        seek_data->eof = feof(fh);
        gfh->state = RW_STATE_NEW;
    }
    clearerr(fh);

    return seek_data;
}