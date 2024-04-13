static int gdBMPPutInt(gdIOCtx *out, int w)
{
	/* Byte order is little-endian */
	gdPutC(w & 0xFF, out);
	gdPutC((w >> 8) & 0xFF, out);
	gdPutC((w >> 16) & 0xFF, out);
	gdPutC((w >> 24) & 0xFF, out);
	return 0;
}