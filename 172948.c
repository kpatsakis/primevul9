void doTest(int w, int h, const int *formats, int nformats, int subsamp,
	char *basename)
{
	tjhandle chandle=NULL, dhandle=NULL;
	unsigned char *dstBuf=NULL;
	unsigned long size=0;  int pfi, pf, i;

	if(!alloc)
		size=tjBufSize(w, h, subsamp);
	if(size!=0)
		if((dstBuf=(unsigned char *)tjAlloc(size))==NULL)
			_throw("Memory allocation failure.");

	if((chandle=tjInitCompress())==NULL || (dhandle=tjInitDecompress())==NULL)
		_throwtj();

	for(pfi=0; pfi<nformats; pfi++)
	{
		for(i=0; i<2; i++)
		{
			int flags=0;
			if(subsamp==TJSAMP_422 || subsamp==TJSAMP_420 || subsamp==TJSAMP_440 ||
				subsamp==TJSAMP_411)
				flags|=TJFLAG_FASTUPSAMPLE;
			if(i==1) flags|=TJFLAG_BOTTOMUP;
			pf=formats[pfi];
			compTest(chandle, &dstBuf, &size, w, h, pf, basename, subsamp, 100,
				flags);
			decompTest(dhandle, dstBuf, size, w, h, pf, basename, subsamp,
				flags);
			if(pf>=TJPF_RGBX && pf<=TJPF_XRGB)
			{
				printf("\n");
				decompTest(dhandle, dstBuf, size, w, h, pf+(TJPF_RGBA-TJPF_RGBX),
					basename, subsamp, flags);
			}
			printf("\n");
		}
	}
	printf("--------------------\n\n");

	bailout:
	if(chandle) tjDestroy(chandle);
	if(dhandle) tjDestroy(dhandle);

	if(dstBuf) tjFree(dstBuf);
}