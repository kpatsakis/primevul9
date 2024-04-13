static int coroutine_fn stat_to_v9stat(V9fsPDU *pdu, V9fsPath *path,
                                       const char *basename,
                                       const struct stat *stbuf,
                                       V9fsStat *v9stat)
{
    int err;

    memset(v9stat, 0, sizeof(*v9stat));

    err = stat_to_qid(pdu, stbuf, &v9stat->qid);
    if (err < 0) {
        return err;
    }
    v9stat->mode = stat_to_v9mode(stbuf);
    v9stat->atime = stbuf->st_atime;
    v9stat->mtime = stbuf->st_mtime;
    v9stat->length = stbuf->st_size;

    v9fs_string_free(&v9stat->uid);
    v9fs_string_free(&v9stat->gid);
    v9fs_string_free(&v9stat->muid);

    v9stat->n_uid = stbuf->st_uid;
    v9stat->n_gid = stbuf->st_gid;
    v9stat->n_muid = 0;

    v9fs_string_free(&v9stat->extension);

    if (v9stat->mode & P9_STAT_MODE_SYMLINK) {
        err = v9fs_co_readlink(pdu, path, &v9stat->extension);
        if (err < 0) {
            return err;
        }
    } else if (v9stat->mode & P9_STAT_MODE_DEVICE) {
        v9fs_string_sprintf(&v9stat->extension, "%c %u %u",
                S_ISCHR(stbuf->st_mode) ? 'c' : 'b',
                major(stbuf->st_rdev), minor(stbuf->st_rdev));
    } else if (S_ISDIR(stbuf->st_mode) || S_ISREG(stbuf->st_mode)) {
        v9fs_string_sprintf(&v9stat->extension, "%s %lu",
                "HARDLINKCOUNT", (unsigned long)stbuf->st_nlink);
    }

    v9fs_string_sprintf(&v9stat->name, "%s", basename);

    v9stat->size = 61 +
        v9fs_string_size(&v9stat->name) +
        v9fs_string_size(&v9stat->uid) +
        v9fs_string_size(&v9stat->gid) +
        v9fs_string_size(&v9stat->muid) +
        v9fs_string_size(&v9stat->extension);
    return 0;
}