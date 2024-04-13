int decomp(unsigned char *srcbuf, unsigned char **jpegbuf,
	unsigned long *jpegsize, unsigned char *dstbuf, int w, int h,
	int subsamp, int jpegqual, char *filename, int tilew, int tileh)
{
	char tempstr[1024], sizestr[20]="\0", qualstr[6]="\0", *ptr;
	FILE *file=NULL;  tjhandle handle=NULL;
	int row, col, iter=0, dstbufalloc=0, retval=0;
	double elapsed, elapsedDecode;
	int ps=tjPixelSize[pf];
	int scaledw=TJSCALED(w, sf);
	int scaledh=TJSCALED(h, sf);
	int pitch=scaledw*ps;
	int ntilesw=(w+tilew-1)/tilew, ntilesh=(h+tileh-1)/tileh;
	unsigned char *dstptr, *dstptr2, *yuvbuf=NULL;

	if(jpegqual>0)
	{
		snprintf(qualstr, 6, "_Q%d", jpegqual);
		qualstr[5]=0;
	}

	if((handle=tjInitDecompress())==NULL)
		_throwtj("executing tjInitDecompress()");

	if(dstbuf==NULL)
	{
		if((dstbuf=(unsigned char *)malloc(pitch*scaledh))==NULL)
			_throwunix("allocating destination buffer");
		dstbufalloc=1;
	}
	/* Set the destination buffer to gray so we know whether the decompressor
	   attempted to write to it */
	memset(dstbuf, 127, pitch*scaledh);

	if(doyuv)
	{
		int width=dotile? tilew:scaledw;
		int height=dotile? tileh:scaledh;
		int yuvsize=tjBufSizeYUV2(width, yuvpad, height, subsamp);
		if((yuvbuf=(unsigned char *)malloc(yuvsize))==NULL)
			_throwunix("allocating YUV buffer");
		memset(yuvbuf, 127, yuvsize);
	}

	/* Benchmark */
	iter=-1;
	elapsed=elapsedDecode=0.;
	while(1)
	{
		int tile=0;
		double start=gettime();
		for(row=0, dstptr=dstbuf; row<ntilesh; row++, dstptr+=pitch*tileh)
		{
			for(col=0, dstptr2=dstptr; col<ntilesw; col++, tile++, dstptr2+=ps*tilew)
			{
				int width=dotile? min(tilew, w-col*tilew):scaledw;
				int height=dotile? min(tileh, h-row*tileh):scaledh;
				if(doyuv)
				{
					double startDecode;
					if(tjDecompressToYUV2(handle, jpegbuf[tile], jpegsize[tile], yuvbuf,
						width, yuvpad, height, flags)==-1)
						_throwtj("executing tjDecompressToYUV2()");
					startDecode=gettime();
					if(tjDecodeYUV(handle, yuvbuf, yuvpad, subsamp, dstptr2, width,
						pitch, height, pf, flags)==-1)
						_throwtj("executing tjDecodeYUV()");
					if(iter>=0) elapsedDecode+=gettime()-startDecode;
				}
				else
					if(tjDecompress2(handle, jpegbuf[tile], jpegsize[tile], dstptr2,
						width, pitch, height, pf, flags)==-1)
						_throwtj("executing tjDecompress2()");
			}
		}
		elapsed+=gettime()-start;
		if(iter>=0)
		{
			iter++;
			if(elapsed>=benchtime) break;
		}
		else if(elapsed>=warmup)
		{
			iter=0;
			elapsed=elapsedDecode=0.;
		}
	}
	if(doyuv) elapsed-=elapsedDecode;

	if(tjDestroy(handle)==-1) _throwtj("executing tjDestroy()");
	handle=NULL;

	if(quiet)
	{
		printf("%-6s%s",
			sigfig((double)(w*h)/1000000.*(double)iter/elapsed, 4, tempstr, 1024),
			quiet==2? "\n":"  ");
		if(doyuv)
			printf("%s\n",
				sigfig((double)(w*h)/1000000.*(double)iter/elapsedDecode, 4, tempstr,
					1024));
		else if(quiet!=2) printf("\n");
	}
	else
	{
		printf("%s --> Frame rate:         %f fps\n",
			doyuv? "Decomp to YUV":"Decompress   ", (double)iter/elapsed);
		printf("                  Throughput:         %f Megapixels/sec\n",
			(double)(w*h)/1000000.*(double)iter/elapsed);
		if(doyuv)
		{
			printf("YUV Decode    --> Frame rate:         %f fps\n",
				(double)iter/elapsedDecode);
			printf("                  Throughput:         %f Megapixels/sec\n",
				(double)(w*h)/1000000.*(double)iter/elapsedDecode);
		}
	}

	if (!dowrite) goto bailout;

	if(sf.num!=1 || sf.denom!=1)
		snprintf(sizestr, 20, "%d_%d", sf.num, sf.denom);
	else if(tilew!=w || tileh!=h)
		snprintf(sizestr, 20, "%dx%d", tilew, tileh);
	else snprintf(sizestr, 20, "full");
	if(decomponly)
		snprintf(tempstr, 1024, "%s_%s.%s", filename, sizestr, ext);
	else
		snprintf(tempstr, 1024, "%s_%s%s_%s.%s", filename, subName[subsamp],
			qualstr, sizestr, ext);

	if(tjSaveImage(tempstr, dstbuf, scaledw, 0, scaledh, pf, flags)==-1)
		_throwtjg("saving bitmap");
	ptr=strrchr(tempstr, '.');
	snprintf(ptr, 1024-(ptr-tempstr), "-err.%s", ext);
	if(srcbuf && sf.num==1 && sf.denom==1)
	{
		if(!quiet) printf("Compression error written to %s.\n", tempstr);
		if(subsamp==TJ_GRAYSCALE)
		{
			int index, index2;
			for(row=0, index=0; row<h; row++, index+=pitch)
			{
				for(col=0, index2=index; col<w; col++, index2+=ps)
				{
					int rindex=index2+tjRedOffset[pf];
					int gindex=index2+tjGreenOffset[pf];
					int bindex=index2+tjBlueOffset[pf];
					int y=(int)((double)srcbuf[rindex]*0.299
						+ (double)srcbuf[gindex]*0.587
						+ (double)srcbuf[bindex]*0.114 + 0.5);
					if(y>255) y=255;
					if(y<0) y=0;
					dstbuf[rindex]=abs(dstbuf[rindex]-y);
					dstbuf[gindex]=abs(dstbuf[gindex]-y);
					dstbuf[bindex]=abs(dstbuf[bindex]-y);
				}
			}
		}
		else
		{
			for(row=0; row<h; row++)
				for(col=0; col<w*ps; col++)
					dstbuf[pitch*row+col]
						=abs(dstbuf[pitch*row+col]-srcbuf[pitch*row+col]);
		}
		if(tjSaveImage(tempstr, dstbuf, w, 0, h, pf, flags)==-1)
			_throwtjg("saving bitmap");
	}

	bailout:
	if(file) fclose(file);
	if(handle) tjDestroy(handle);
	if(dstbuf && dstbufalloc) free(dstbuf);
	if(yuvbuf) free(yuvbuf);
	return retval;
}