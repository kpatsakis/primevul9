static int stat_to_qid(V9fsPDU *pdu, const struct stat *stbuf, V9fsQID *qidp)
{
    int err;
    size_t size;

    if (pdu->s->ctx.export_flags & V9FS_REMAP_INODES) {
        /* map inode+device to qid path (fast path) */
        err = qid_path_suffixmap(pdu, stbuf, &qidp->path);
        if (err == -ENFILE) {
            /* fast path didn't work, fall back to full map */
            err = qid_path_fullmap(pdu, stbuf, &qidp->path);
        }
        if (err) {
            return err;
        }
    } else {
        if (pdu->s->dev_id != stbuf->st_dev) {
            if (pdu->s->ctx.export_flags & V9FS_FORBID_MULTIDEVS) {
                error_report_once(
                    "9p: Multiple devices detected in same VirtFS export. "
                    "Access of guest to additional devices is (partly) "
                    "denied due to virtfs option 'multidevs=forbid' being "
                    "effective."
                );
                return -ENODEV;
            } else {
                warn_report_once(
                    "9p: Multiple devices detected in same VirtFS export, "
                    "which might lead to file ID collisions and severe "
                    "misbehaviours on guest! You should either use a "
                    "separate export for each device shared from host or "
                    "use virtfs option 'multidevs=remap'!"
                );
            }
        }
        memset(&qidp->path, 0, sizeof(qidp->path));
        size = MIN(sizeof(stbuf->st_ino), sizeof(qidp->path));
        memcpy(&qidp->path, &stbuf->st_ino, size);
    }

    qidp->version = stbuf->st_mtime ^ (stbuf->st_size << 8);
    qidp->type = 0;
    if (S_ISDIR(stbuf->st_mode)) {
        qidp->type |= P9_QID_TYPE_DIR;
    }
    if (S_ISLNK(stbuf->st_mode)) {
        qidp->type |= P9_QID_TYPE_SYMLINK;
    }

    return 0;
}