static int bmp_read_header(gdIOCtx *infile, bmp_hdr_t *hdr)
{
	if(
	    !gdGetWordLSB(&hdr->magic, infile) ||
	    !gdGetIntLSB(&hdr->size, infile) ||
	    !gdGetWordLSB(&hdr->reserved1, infile) ||
	    !gdGetWordLSB(&hdr->reserved2 , infile) ||
	    !gdGetIntLSB(&hdr->off , infile)
	) {
		return 1;
	}
	return 0;
}