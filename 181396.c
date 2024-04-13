static int donttouch_stat(V9fsStat *stat)
{
    if (stat->type == -1 &&
        stat->dev == -1 &&
        stat->qid.type == 0xff &&
        stat->qid.version == (uint32_t) -1 &&
        stat->qid.path == (uint64_t) -1 &&
        stat->mode == -1 &&
        stat->atime == -1 &&
        stat->mtime == -1 &&
        stat->length == -1 &&
        !stat->name.size &&
        !stat->uid.size &&
        !stat->gid.size &&
        !stat->muid.size &&
        stat->n_uid == -1 &&
        stat->n_gid == -1 &&
        stat->n_muid == -1) {
        return 1;
    }

    return 0;
}