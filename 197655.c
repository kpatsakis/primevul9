static int fill_in_write_vector(struct kvec *vec, struct nfsd4_write *write)
{
        int i = 1;
        int buflen = write->wr_buflen;

        vec[0].iov_base = write->wr_head.iov_base;
        vec[0].iov_len = min_t(int, buflen, write->wr_head.iov_len);
        buflen -= vec[0].iov_len;

        while (buflen) {
                vec[i].iov_base = page_address(write->wr_pagelist[i - 1]);
                vec[i].iov_len = min_t(int, PAGE_SIZE, buflen);
                buflen -= vec[i].iov_len;
                i++;
        }
        return i;
}