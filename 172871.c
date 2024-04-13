int doBmpTest(const char *ext, int width, int align, int height, int pf,
	int flags)
{
	char filename[80], *md5sum, md5buf[65];
	int ps=tjPixelSize[pf], pitch=PAD(width*ps, align),
		loadWidth=0, loadHeight=0, retval=0;
	unsigned char *buf=NULL;
	char *md5ref;

	if(pf==TJPF_GRAY)
	{
		md5ref=!strcasecmp(ext, "ppm")? "bc77dea8eaf006aa187582b301f67e02":
			"2670a3f8cf19d855183c02ccf18d2a35";
	}
	else
	{
		md5ref=!strcasecmp(ext, "ppm")? "c0c9f772b464d1896326883a5c79c545":
			"6d659071b9bfcdee2def22cb58ddadca";
	}

	if((buf=(unsigned char *)tjAlloc(pitch*height))==NULL)
		_throw("Could not allocate memory");
	initBitmap(buf, width, pitch, height, pf, flags);

	snprintf(filename, 80, "test_bmp_%s_%d_%s.%s", pixFormatStr[pf], align,
		(flags&TJFLAG_BOTTOMUP)? "bu":"td", ext);
	_tj(tjSaveImage(filename, buf, width, pitch, height, pf, flags));
	md5sum=MD5File(filename, md5buf);
	if(strcasecmp(md5sum, md5ref))
		_throwmd5(filename, md5sum, md5ref);

	tjFree(buf);  buf=NULL;
	if((buf=tjLoadImage(filename, &loadWidth, align, &loadHeight, &pf,
		flags))==NULL)
		_throwtj();
	if(width!=loadWidth || height!=loadHeight)
	{
		printf("\n   Image dimensions of %s are bogus\n", filename);
		retval=-1;  goto bailout;
	}
	if(!cmpBitmap(buf, width, pitch, height, pf, flags, 0))
	{
		printf("\n   Pixel data in %s is bogus\n", filename);
		retval=-1;  goto bailout;
	}
	if(pf==TJPF_GRAY)
	{
		tjFree(buf);  buf=NULL;
		pf=TJPF_XBGR;
		if((buf=tjLoadImage(filename, &loadWidth, align, &loadHeight, &pf,
			flags))==NULL)
			_throwtj();
		pitch=PAD(width*tjPixelSize[pf], align);
		if(!cmpBitmap(buf, width, pitch, height, pf, flags, 1))
		{
			printf("\n   Converting %s to RGB failed\n", filename);
			retval=-1;  goto bailout;
		}

		tjFree(buf);  buf=NULL;
		pf=TJPF_CMYK;
		if((buf=tjLoadImage(filename, &loadWidth, align, &loadHeight, &pf,
			flags))==NULL)
			_throwtj();
		pitch=PAD(width*tjPixelSize[pf], align);
		if(!cmpBitmap(buf, width, pitch, height, pf, flags, 1))
		{
			printf("\n   Converting %s to CMYK failed\n", filename);
			retval=-1;  goto bailout;
		}
	}
	else if(pf!=TJPF_CMYK)
	{
		tjFree(buf);  buf=NULL;
		pf=TJPF_GRAY;
		if((buf=tjLoadImage(filename, &loadWidth, align, &loadHeight, &pf,
			flags))==NULL)
			_throwtj();
		pitch=PAD(width, align);
		if(!cmpBitmap(buf, width, pitch, height, pf, flags, 0))
		{
			printf("\n   Converting %s to grayscale failed\n", filename);
			retval=-1;  goto bailout;
		}
	}
	unlink(filename);

	bailout:
	if(buf) tjFree(buf);
	if(exitStatus<0) return exitStatus;
	return retval;
}