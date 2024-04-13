static int v9fs_xattr_write(V9fsState *s, V9fsPDU *pdu, V9fsFidState *fidp,
                            uint64_t off, uint32_t count,
                            struct iovec *sg, int cnt)
{
    int i, to_copy;
    ssize_t err = 0;
    uint64_t write_count;
    size_t offset = 7;


    if (fidp->fs.xattr.len < off) {
        return -ENOSPC;
    }
    write_count = fidp->fs.xattr.len - off;
    if (write_count > count) {
        write_count = count;
    }
    err = pdu_marshal(pdu, offset, "d", write_count);
    if (err < 0) {
        return err;
    }
    err += offset;
    fidp->fs.xattr.copied_len += write_count;
    /*
     * Now copy the content from sg list
     */
    for (i = 0; i < cnt; i++) {
        if (write_count > sg[i].iov_len) {
            to_copy = sg[i].iov_len;
        } else {
            to_copy = write_count;
        }
        memcpy((char *)fidp->fs.xattr.value + off, sg[i].iov_base, to_copy);
        /* updating vs->off since we are not using below */
        off += to_copy;
        write_count -= to_copy;
    }

    return err;
}