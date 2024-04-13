static int gdBMPPutWord(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	return 0;
}