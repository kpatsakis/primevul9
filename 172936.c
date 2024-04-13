int fullTest(unsigned char *srcbuf, int w, int h, int subsamp, int jpegqual,
	char *filename)
{
	char tempstr[1024], tempstr2[80];
	FILE *file=NULL;  tjhandle handle=NULL;
	unsigned char **jpegbuf=NULL, *yuvbuf=NULL, *tmpbuf=NULL, *srcptr, *srcptr2;
	double start, elapsed, elapsedEncode;
	int totaljpegsize=0, row, col, i, tilew=w, tileh=h, retval=0;
	int iter, yuvsize=0;
	unsigned long *jpegsize=NULL;
	int ps=tjPixelSize[pf];
	int ntilesw=1, ntilesh=1, pitch=w*ps;
	const char *pfStr=pixFormatStr[pf];

	if((tmpbuf=(unsigned char *)malloc(pitch*h)) == NULL)
		_throwunix("allocating temporary image buffer");

	if(!quiet)
		printf(">>>>>  %s (%s) <--> JPEG %s Q%d  <<<<<\n", pfStr,
			(flags&TJFLAG_BOTTOMUP)? "Bottom-up":"Top-down", subNameLong[subsamp],
			jpegqual);

	for(tilew=dotile? 8:w, tileh=dotile? 8:h; ; tilew*=2, tileh*=2)
	{
		if(tilew>w) tilew=w;
		if(tileh>h) tileh=h;
		ntilesw=(w+tilew-1)/tilew;  ntilesh=(h+tileh-1)/tileh;

		if((jpegbuf=(unsigned char **)malloc(sizeof(unsigned char *)
			*ntilesw*ntilesh))==NULL)
			_throwunix("allocating JPEG tile array");
		memset(jpegbuf, 0, sizeof(unsigned char *)*ntilesw*ntilesh);
		if((jpegsize=(unsigned long *)malloc(sizeof(unsigned long)
			*ntilesw*ntilesh))==NULL)
			_throwunix("allocating JPEG size array");
		memset(jpegsize, 0, sizeof(unsigned long)*ntilesw*ntilesh);

		if((flags&TJFLAG_NOREALLOC)!=0)
			for(i=0; i<ntilesw*ntilesh; i++)
			{
				if((jpegbuf[i]=(unsigned char *)tjAlloc(tjBufSize(tilew, tileh,
					subsamp)))==NULL)
					_throwunix("allocating JPEG tiles");
			}

		/* Compression test */
		if(quiet==1)
			printf("%-4s (%s)  %-5s    %-3d   ", pfStr,
				(flags&TJFLAG_BOTTOMUP)? "BU":"TD", subNameLong[subsamp], jpegqual);
		for(i=0; i<h; i++)
			memcpy(&tmpbuf[pitch*i], &srcbuf[w*ps*i], w*ps);
		if((handle=tjInitCompress())==NULL)
			_throwtj("executing tjInitCompress()");

		if(doyuv)
		{
			yuvsize=tjBufSizeYUV2(tilew, yuvpad, tileh, subsamp);
			if((yuvbuf=(unsigned char *)malloc(yuvsize))==NULL)
				_throwunix("allocating YUV buffer");
			memset(yuvbuf, 127, yuvsize);
		}

		/* Benchmark */
		iter=-1;
		elapsed=elapsedEncode=0.;
		while(1)
		{
			int tile=0;
			totaljpegsize=0;
			start=gettime();
			for(row=0, srcptr=srcbuf; row<ntilesh; row++, srcptr+=pitch*tileh)
			{
				for(col=0, srcptr2=srcptr; col<ntilesw; col++, tile++,
					srcptr2+=ps*tilew)
				{
					int width=min(tilew, w-col*tilew);
					int height=min(tileh, h-row*tileh);
					if(doyuv)
					{
						double startEncode=gettime();
						if(tjEncodeYUV3(handle, srcptr2, width, pitch, height, pf, yuvbuf,
							yuvpad, subsamp, flags)==-1)
							_throwtj("executing tjEncodeYUV3()");
						if(iter>=0) elapsedEncode+=gettime()-startEncode;
						if(tjCompressFromYUV(handle, yuvbuf, width, yuvpad, height,
							subsamp, &jpegbuf[tile], &jpegsize[tile], jpegqual, flags)==-1)
							_throwtj("executing tjCompressFromYUV()");
					}
					else
					{
						if(tjCompress2(handle, srcptr2, width, pitch, height, pf,
							&jpegbuf[tile], &jpegsize[tile], subsamp, jpegqual, flags)==-1)
							_throwtj("executing tjCompress2()");
					}
					totaljpegsize+=jpegsize[tile];
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
				elapsed=elapsedEncode=0.;
			}
		}
		if(doyuv) elapsed-=elapsedEncode;

		if(tjDestroy(handle)==-1) _throwtj("executing tjDestroy()");
		handle=NULL;

		if(quiet==1) printf("%-5d  %-5d   ", tilew, tileh);
		if(quiet)
		{
			if(doyuv)
				printf("%-6s%s",
					sigfig((double)(w*h)/1000000.*(double)iter/elapsedEncode, 4, tempstr,
						1024), quiet==2? "\n":"  ");
			printf("%-6s%s",
				sigfig((double)(w*h)/1000000.*(double)iter/elapsed, 4,	tempstr, 1024),
				quiet==2? "\n":"  ");
			printf("%-6s%s",
				sigfig((double)(w*h*ps)/(double)totaljpegsize, 4, tempstr2, 80),
				quiet==2? "\n":"  ");
		}
		else
		{
			printf("\n%s size: %d x %d\n", dotile? "Tile":"Image", tilew,
				tileh);
			if(doyuv)
			{
				printf("Encode YUV    --> Frame rate:         %f fps\n",
					(double)iter/elapsedEncode);
				printf("                  Output image size:  %d bytes\n", yuvsize);
				printf("                  Compression ratio:  %f:1\n",
					(double)(w*h*ps)/(double)yuvsize);
				printf("                  Throughput:         %f Megapixels/sec\n",
					(double)(w*h)/1000000.*(double)iter/elapsedEncode);
				printf("                  Output bit stream:  %f Megabits/sec\n",
					(double)yuvsize*8./1000000.*(double)iter/elapsedEncode);
			}
			printf("%s --> Frame rate:         %f fps\n",
				doyuv? "Comp from YUV":"Compress     ", (double)iter/elapsed);
			printf("                  Output image size:  %d bytes\n",
				totaljpegsize);
			printf("                  Compression ratio:  %f:1\n",
				(double)(w*h*ps)/(double)totaljpegsize);
			printf("                  Throughput:         %f Megapixels/sec\n",
				(double)(w*h)/1000000.*(double)iter/elapsed);
			printf("                  Output bit stream:  %f Megabits/sec\n",
				(double)totaljpegsize*8./1000000.*(double)iter/elapsed);
		}
		if(tilew==w && tileh==h && dowrite)
		{
			snprintf(tempstr, 1024, "%s_%s_Q%d.jpg", filename, subName[subsamp],
				jpegqual);
			if((file=fopen(tempstr, "wb"))==NULL)
				_throwunix("opening reference image");
			if(fwrite(jpegbuf[0], jpegsize[0], 1, file)!=1)
				_throwunix("writing reference image");
			fclose(file);  file=NULL;
			if(!quiet) printf("Reference image written to %s\n", tempstr);
		}

		/* Decompression test */
		if(!componly)
		{
			if(decomp(srcbuf, jpegbuf, jpegsize, tmpbuf, w, h, subsamp, jpegqual,
				filename, tilew, tileh)==-1)
				goto bailout;
		}

		for(i=0; i<ntilesw*ntilesh; i++)
		{
			if(jpegbuf[i]) tjFree(jpegbuf[i]);
			jpegbuf[i]=NULL;
		}
		free(jpegbuf);  jpegbuf=NULL;
		free(jpegsize);  jpegsize=NULL;
		if(doyuv)
		{
			free(yuvbuf);  yuvbuf=NULL;
		}

		if(tilew==w && tileh==h) break;
	}

	bailout:
	if(file) {fclose(file);  file=NULL;}
	if(jpegbuf)
	{
		for(i=0; i<ntilesw*ntilesh; i++)
		{
			if(jpegbuf[i]) tjFree(jpegbuf[i]);
			jpegbuf[i]=NULL;
		}
		free(jpegbuf);  jpegbuf=NULL;
	}
	if(yuvbuf) {free(yuvbuf);  yuvbuf=NULL;}
	if(jpegsize) {free(jpegsize);  jpegsize=NULL;}
	if(tmpbuf) {free(tmpbuf);  tmpbuf=NULL;}
	if(handle) {tjDestroy(handle);  handle=NULL;}
	return retval;
}