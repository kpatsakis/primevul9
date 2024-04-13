static V9fsFidState *alloc_fid(V9fsState *s, int32_t fid)
{
    V9fsFidState *f;

    for (f = s->fid_list; f; f = f->next) {
        /* If fid is already there return NULL */
        BUG_ON(f->clunked);
        if (f->fid == fid) {
            return NULL;
        }
    }
    f = g_malloc0(sizeof(V9fsFidState));
    f->fid = fid;
    f->fid_type = P9_FID_NONE;
    f->ref = 1;
    /*
     * Mark the fid as referenced so that the LRU
     * reclaim won't close the file descriptor
     */
    f->flags |= FID_REFERENCED;
    f->next = s->fid_list;
    s->fid_list = f;

    v9fs_readdir_init(s->proto_version, &f->fs.dir);
    v9fs_readdir_init(s->proto_version, &f->fs_reclaim.dir);

    return f;
}