static V9fsFidState *coroutine_fn get_fid(V9fsPDU *pdu, int32_t fid)
{
    int err;
    V9fsFidState *f;
    V9fsState *s = pdu->s;

    for (f = s->fid_list; f; f = f->next) {
        BUG_ON(f->clunked);
        if (f->fid == fid) {
            /*
             * Update the fid ref upfront so that
             * we don't get reclaimed when we yield
             * in open later.
             */
            f->ref++;
            /*
             * check whether we need to reopen the
             * file. We might have closed the fd
             * while trying to free up some file
             * descriptors.
             */
            err = v9fs_reopen_fid(pdu, f);
            if (err < 0) {
                f->ref--;
                return NULL;
            }
            /*
             * Mark the fid as referenced so that the LRU
             * reclaim won't close the file descriptor
             */
            f->flags |= FID_REFERENCED;
            return f;
        }
    }
    return NULL;
}