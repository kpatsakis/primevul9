void decompTest(tjhandle handle, unsigned char *jpegBuf,
	unsigned long jpegSize, int w, int h, int pf, char *basename, int subsamp,
	int flags)
{
	int i, n=0;
	tjscalingfactor *sf=tjGetScalingFactors(&n);
	if(!sf || !n) _throwtj();

	for(i=0; i<n; i++)
	{
		if(subsamp==TJSAMP_444 || subsamp==TJSAMP_GRAY ||
			(subsamp==TJSAMP_411 && sf[i].num==1 &&
				(sf[i].denom==2 || sf[i].denom==1)) ||
			(subsamp!=TJSAMP_411 && sf[i].num==1 &&
				(sf[i].denom==4 || sf[i].denom==2 || sf[i].denom==1)))
			_decompTest(handle, jpegBuf, jpegSize, w, h, pf, basename, subsamp,
				flags, sf[i]);
	}

	bailout:
	return;
}