uint32_t bs_close_read (Bitstream *bs)
{
    uint32_t bytes_read;

    if (bs->bc < sizeof (*(bs->ptr)) * 8)
        bs->ptr++;

    bytes_read = (uint32_t)(bs->ptr - bs->buf) * sizeof (*(bs->ptr));

    if (!(bytes_read & 1))
        ++bytes_read;

    CLEAR (*bs);
    return bytes_read;
}