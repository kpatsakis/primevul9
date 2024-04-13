static int coroutine_fn v9fs_do_readdir(V9fsPDU *pdu, V9fsFidState *fidp,
                                        off_t offset, int32_t max_count)
{
    size_t size;
    V9fsQID qid;
    V9fsString name;
    int len, err = 0;
    int32_t count = 0;
    struct dirent *dent;
    struct stat *st;
    struct V9fsDirEnt *entries = NULL;

    /*
     * inode remapping requires the device id, which in turn might be
     * different for different directory entries, so if inode remapping is
     * enabled we have to make a full stat for each directory entry
     */
    const bool dostat = pdu->s->ctx.export_flags & V9FS_REMAP_INODES;

    /*
     * Fetch all required directory entries altogether on a background IO
     * thread from fs driver. We don't want to do that for each entry
     * individually, because hopping between threads (this main IO thread
     * and background IO driver thread) would sum up to huge latencies.
     */
    count = v9fs_co_readdir_many(pdu, fidp, &entries, offset, max_count,
                                 dostat);
    if (count < 0) {
        err = count;
        count = 0;
        goto out;
    }
    count = 0;

    for (struct V9fsDirEnt *e = entries; e; e = e->next) {
        dent = e->dent;

        if (pdu->s->ctx.export_flags & V9FS_REMAP_INODES) {
            st = e->st;
            /* e->st should never be NULL, but just to be sure */
            if (!st) {
                err = -1;
                break;
            }

            /* remap inode */
            err = stat_to_qid(pdu, st, &qid);
            if (err < 0) {
                break;
            }
        } else {
            /*
             * Fill up just the path field of qid because the client uses
             * only that. To fill the entire qid structure we will have
             * to stat each dirent found, which is expensive. For the
             * latter reason we don't call stat_to_qid() here. Only drawback
             * is that no multi-device export detection of stat_to_qid()
             * would be done and provided as error to the user here. But
             * user would get that error anyway when accessing those
             * files/dirs through other ways.
             */
            size = MIN(sizeof(dent->d_ino), sizeof(qid.path));
            memcpy(&qid.path, &dent->d_ino, size);
            /* Fill the other fields with dummy values */
            qid.type = 0;
            qid.version = 0;
        }

        v9fs_string_init(&name);
        v9fs_string_sprintf(&name, "%s", dent->d_name);

        /* 11 = 7 + 4 (7 = start offset, 4 = space for storing count) */
        len = pdu_marshal(pdu, 11 + count, "Qqbs",
                          &qid, dent->d_off,
                          dent->d_type, &name);

        v9fs_string_free(&name);

        if (len < 0) {
            err = len;
            break;
        }

        count += len;
    }

out:
    v9fs_free_dirents(entries);
    if (err < 0) {
        return err;
    }
    return count;
}