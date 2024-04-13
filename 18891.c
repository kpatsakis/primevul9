out_char_nf(int c)
{
    out_buf[out_pos++] = (unsigned)c;

    if (out_pos >= OUT_SIZE)
	out_flush();
    return (unsigned)c;
}