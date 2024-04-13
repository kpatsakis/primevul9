DLLEXPORT unsigned long DLLCALL TJBUFSIZE(int width, int height)
{
	unsigned long retval=0;
	if(width<1 || height<1)
		_throwg("TJBUFSIZE(): Invalid argument");

	/* This allows for rare corner cases in which a JPEG image can actually be
	   larger than the uncompressed input (we wouldn't mention it if it hadn't
	   happened before.) */
	retval=PAD(width, 16) * PAD(height, 16) * 6 + 2048;

	bailout:
	return retval;
}