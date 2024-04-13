static int v9fs_fill_statfs(V9fsState *s, V9fsPDU *pdu, struct statfs *stbuf)
{
    uint32_t f_type;
    uint32_t f_bsize;
    uint64_t f_blocks;
    uint64_t f_bfree;
    uint64_t f_bavail;
    uint64_t f_files;
    uint64_t f_ffree;
    uint64_t fsid_val;
    uint32_t f_namelen;
    size_t offset = 7;
    int32_t bsize_factor;

    /*
     * compute bsize factor based on host file system block size
     * and client msize
     */
    bsize_factor = (s->msize - P9_IOHDRSZ) / stbuf->f_bsize;
    if (!bsize_factor) {
        bsize_factor = 1;
    }
    f_type  = stbuf->f_type;
    f_bsize = stbuf->f_bsize;
    f_bsize *= bsize_factor;
    /*
     * f_bsize is adjusted(multiplied) by bsize factor, so we need to
     * adjust(divide) the number of blocks, free blocks and available
     * blocks by bsize factor
     */
    f_blocks = stbuf->f_blocks / bsize_factor;
    f_bfree  = stbuf->f_bfree / bsize_factor;
    f_bavail = stbuf->f_bavail / bsize_factor;
    f_files  = stbuf->f_files;
    f_ffree  = stbuf->f_ffree;
    fsid_val = (unsigned int) stbuf->f_fsid.__val[0] |
               (unsigned long long)stbuf->f_fsid.__val[1] << 32;
    f_namelen = stbuf->f_namelen;

    return pdu_marshal(pdu, offset, "ddqqqqqqd",
                       f_type, f_bsize, f_blocks, f_bfree,
                       f_bavail, f_files, f_ffree,
                       fsid_val, f_namelen);
}