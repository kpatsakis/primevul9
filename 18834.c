out_str_nf(char_u *s)
{
    // avoid terminal strings being split up
    if (out_pos > OUT_SIZE - MAX_ESC_SEQ_LEN)
	out_flush();

    while (*s)
	out_char_nf(*s++);

    // For testing we write one string at a time.
    if (p_wd)
	out_flush();
}