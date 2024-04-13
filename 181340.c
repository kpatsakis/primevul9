static int32_t coroutine_fn get_iounit(V9fsPDU *pdu, V9fsPath *path)
{
    struct statfs stbuf;
    int32_t iounit = 0;
    V9fsState *s = pdu->s;

    /*
     * iounit should be multiples of f_bsize (host filesystem block size
     * and as well as less than (client msize - P9_IOHDRSZ))
     */
    if (!v9fs_co_statfs(pdu, path, &stbuf)) {
        if (stbuf.f_bsize) {
            iounit = stbuf.f_bsize;
            iounit *= (s->msize - P9_IOHDRSZ) / stbuf.f_bsize;
        }
    }
    if (!iounit) {
        iounit = s->msize - P9_IOHDRSZ;
    }
    return iounit;
}