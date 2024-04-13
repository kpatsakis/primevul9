static void bs_open_read (Bitstream *bs, void *buffer_start, void *buffer_end)
{
    bs->error = bs->sr = bs->bc = 0;
    bs->ptr = (bs->buf = buffer_start) - 1;
    bs->end = buffer_end;
    bs->wrap = bs_read;
}