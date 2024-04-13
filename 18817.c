out_char(unsigned c)
{
#if defined(UNIX) || defined(VMS) || defined(AMIGA) || defined(MACOS_X)
    if (c == '\n')	// turn LF into CR-LF (CRMOD doesn't seem to do this)
	out_char('\r');
#endif

    out_buf[out_pos++] = c;

    // For testing we flush each time.
    if (out_pos >= OUT_SIZE || p_wd)
	out_flush();
}