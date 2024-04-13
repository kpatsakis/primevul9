static void bs_read (Bitstream *bs)
{
    bs->ptr = bs->buf;
    bs->error = 1;
}