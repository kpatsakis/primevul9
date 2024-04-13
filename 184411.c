static uint32_t read_next_header (FILE *infile, WavpackHeader *wphdr)
{
    unsigned char buffer [sizeof (*wphdr)], *sp = buffer + sizeof (*wphdr), *ep = sp;
    uint32_t bytes_skipped = 0;
    int bleft;

    while (1) {
	if (sp < ep) {
	    bleft = (int)(ep - sp);
	    memmove (buffer, sp, bleft);
	}
	else
	    bleft = 0;

	if (fread (buffer + bleft, 1, sizeof (*wphdr) - bleft, infile) != (int32_t) sizeof (*wphdr) - bleft)
	    return -1;

	sp = buffer;

	if (*sp++ == 'w' && *sp == 'v' && *++sp == 'p' && *++sp == 'k' &&
            !(*++sp & 1) && sp [2] < 16 && !sp [3] && (sp [2] || sp [1] || *sp >= 24) && sp [5] == 4 &&
            sp [4] >= (MIN_STREAM_VERS & 0xff) && sp [4] <= (MAX_STREAM_VERS & 0xff) && sp [18] < 3 && !sp [19]) {
		memcpy (wphdr, buffer, sizeof (*wphdr));
		WavpackLittleEndianToNative (wphdr, WavpackHeaderFormat);
		return bytes_skipped;
	    }

	while (sp < ep && *sp != 'w')
	    sp++;

	if ((bytes_skipped += (uint32_t)(sp - buffer)) > 1024 * 1024)
	    return -1;
    }
}