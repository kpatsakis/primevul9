static int coroutine_fn v9fs_reopen_fid(V9fsPDU *pdu, V9fsFidState *f)
{
    int err = 1;
    if (f->fid_type == P9_FID_FILE) {
        if (f->fs.fd == -1) {
            do {
                err = v9fs_co_open(pdu, f, f->open_flags);
            } while (err == -EINTR && !pdu->cancelled);
        }
    } else if (f->fid_type == P9_FID_DIR) {
        if (f->fs.dir.stream == NULL) {
            do {
                err = v9fs_co_opendir(pdu, f);
            } while (err == -EINTR && !pdu->cancelled);
        }
    }
    return err;
}