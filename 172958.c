int decompTest(char *filename)
{
	FILE *file=NULL;  tjhandle handle=NULL;
	unsigned char **jpegbuf=NULL, *srcbuf=NULL;
	unsigned long *jpegsize=NULL, srcsize, totaljpegsize;
	tjtransform *t=NULL;
	int w=0, h=0, subsamp=-1, cs=-1, _w, _h, _tilew, _tileh,
		_ntilesw, _ntilesh, _subsamp;
	char *temp=NULL, tempstr[80], tempstr2[80];
	int row, col, i, iter, tilew, tileh, ntilesw=1, ntilesh=1, retval=0;
	double start, elapsed;
	int ps=tjPixelSize[pf], tile, decompsrc=0;

	if((file=fopen(filename, "rb"))==NULL)
		_throwunix("opening file");
	if(fseek(file, 0, SEEK_END)<0 || (srcsize=ftell(file))==(unsigned long)-1)
		_throwunix("determining file size");
	if((srcbuf=(unsigned char *)malloc(srcsize))==NULL)
		_throwunix("allocating memory");
	if(fseek(file, 0, SEEK_SET)<0)
		_throwunix("setting file position");
	if(fread(srcbuf, srcsize, 1, file)<1)
		_throwunix("reading JPEG data");
	fclose(file);  file=NULL;

	temp=strrchr(filename, '.');
	if(temp!=NULL) *temp='\0';

	if((handle=tjInitTransform())==NULL)
		_throwtj("executing tjInitTransform()");
	if(tjDecompressHeader3(handle, srcbuf, srcsize, &w, &h, &subsamp, &cs)==-1)
		_throwtj("executing tjDecompressHeader3()");
	if(cs==TJCS_YCCK || cs==TJCS_CMYK)
	{
		pf=TJPF_CMYK;  ps=tjPixelSize[pf];
	}

	if(quiet==1)
	{
		printf("All performance values in Mpixels/sec\n\n");
		printf("Bitmap     JPEG   JPEG     %s  %s   Xform   Comp    Decomp  ",
			dotile? "Tile ":"Image", dotile? "Tile ":"Image");
		if(doyuv) printf("Decode");
		printf("\n");
		printf("Format     CS     Subsamp  Width  Height  Perf    Ratio   Perf    ");
		if(doyuv) printf("Perf");
		printf("\n\n");
	}
	else if(!quiet)
		printf(">>>>>  JPEG %s --> %s (%s)  <<<<<\n",
			formatName(subsamp, cs, tempstr), pixFormatStr[pf],
			(flags&TJFLAG_BOTTOMUP)? "Bottom-up":"Top-down");

	for(tilew=dotile? 16:w, tileh=dotile? 16:h; ; tilew*=2, tileh*=2)
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

		if((flags&TJFLAG_NOREALLOC)!=0 || !dotile)
			for(i=0; i<ntilesw*ntilesh; i++)
			{
				if((jpegbuf[i]=(unsigned char *)tjAlloc(tjBufSize(tilew, tileh,
					subsamp)))==NULL)
					_throwunix("allocating JPEG tiles");
			}

		_w=w;  _h=h;  _tilew=tilew;  _tileh=tileh;
		if(!quiet)
		{
			printf("\n%s size: %d x %d", dotile? "Tile":"Image", _tilew,
				_tileh);
			if(sf.num!=1 || sf.denom!=1)
				printf(" --> %d x %d", TJSCALED(_w, sf), TJSCALED(_h, sf));
			printf("\n");
		}
		else if(quiet==1)
		{
			printf("%-4s (%s)  %-5s  %-5s    ", pixFormatStr[pf],
				(flags&TJFLAG_BOTTOMUP)? "BU":"TD", csName[cs], subNameLong[subsamp]);
			printf("%-5d  %-5d   ", tilew, tileh);
		}

		_subsamp=subsamp;
		if(dotile || xformop!=TJXOP_NONE || xformopt!=0 || customFilter)
		{
			if((t=(tjtransform *)malloc(sizeof(tjtransform)*ntilesw*ntilesh))
				==NULL)
				_throwunix("allocating image transform array");

			if(xformop==TJXOP_TRANSPOSE || xformop==TJXOP_TRANSVERSE
				|| xformop==TJXOP_ROT90 || xformop==TJXOP_ROT270)
			{
				_w=h;  _h=w;  _tilew=tileh;  _tileh=tilew;
			}

			if(xformopt&TJXOPT_GRAY) _subsamp=TJ_GRAYSCALE;
			if(xformop==TJXOP_HFLIP || xformop==TJXOP_ROT180)
				_w=_w-(_w%tjMCUWidth[_subsamp]);
			if(xformop==TJXOP_VFLIP || xformop==TJXOP_ROT180)
				_h=_h-(_h%tjMCUHeight[_subsamp]);
			if(xformop==TJXOP_TRANSVERSE || xformop==TJXOP_ROT90)
				_w=_w-(_w%tjMCUHeight[_subsamp]);
			if(xformop==TJXOP_TRANSVERSE || xformop==TJXOP_ROT270)
				_h=_h-(_h%tjMCUWidth[_subsamp]);
			_ntilesw=(_w+_tilew-1)/_tilew;
			_ntilesh=(_h+_tileh-1)/_tileh;

			if(xformop==TJXOP_TRANSPOSE || xformop==TJXOP_TRANSVERSE
				|| xformop==TJXOP_ROT90 || xformop==TJXOP_ROT270)
			{
				if(_subsamp==TJSAMP_422) _subsamp=TJSAMP_440;
				else if(_subsamp==TJSAMP_440) _subsamp=TJSAMP_422;
			}

			for(row=0, tile=0; row<_ntilesh; row++)
			{
				for(col=0; col<_ntilesw; col++, tile++)
				{
					t[tile].r.w=min(_tilew, _w-col*_tilew);
					t[tile].r.h=min(_tileh, _h-row*_tileh);
					t[tile].r.x=col*_tilew;
					t[tile].r.y=row*_tileh;
					t[tile].op=xformop;
					t[tile].options=xformopt|TJXOPT_TRIM;
					t[tile].customFilter=customFilter;
					if(t[tile].options&TJXOPT_NOOUTPUT && jpegbuf[tile])
					{
						tjFree(jpegbuf[tile]);  jpegbuf[tile]=NULL;
					}
				}
			}

			iter=-1;
			elapsed=0.;
			while(1)
			{
				start=gettime();
				if(tjTransform(handle, srcbuf, srcsize, _ntilesw*_ntilesh, jpegbuf,
					jpegsize, t, flags)==-1)
					_throwtj("executing tjTransform()");
				elapsed+=gettime()-start;
				if(iter>=0)
				{
					iter++;
					if(elapsed>=benchtime) break;
				}
				else if(elapsed>=warmup)
				{
					iter=0;
					elapsed=0.;
				}
			}

			free(t);  t=NULL;

			for(tile=0, totaljpegsize=0; tile<_ntilesw*_ntilesh; tile++)
				totaljpegsize+=jpegsize[tile];

			if(quiet)
			{
				printf("%-6s%s%-6s%s",
					sigfig((double)(w*h)/1000000./elapsed, 4, tempstr, 80),
					quiet==2? "\n":"  ",
					sigfig((double)(w*h*ps)/(double)totaljpegsize, 4, tempstr2, 80),
					quiet==2? "\n":"  ");
			}
			else if(!quiet)
			{
				printf("Transform     --> Frame rate:         %f fps\n", 1.0/elapsed);
				printf("                  Output image size:  %lu bytes\n", totaljpegsize);
				printf("                  Compression ratio:  %f:1\n",
					(double)(w*h*ps)/(double)totaljpegsize);
				printf("                  Throughput:         %f Megapixels/sec\n",
					(double)(w*h)/1000000./elapsed);
				printf("                  Output bit stream:  %f Megabits/sec\n",
					(double)totaljpegsize*8./1000000./elapsed);
			}
		}
		else
		{
			if(quiet==1) printf("N/A     N/A     ");
			tjFree(jpegbuf[0]);
			jpegbuf[0]=NULL;
			decompsrc=1;
		}

		if(w==tilew) _tilew=_w;
		if(h==tileh) _tileh=_h;
		if(!(xformopt&TJXOPT_NOOUTPUT))
		{
			if(decomp(NULL, decompsrc? &srcbuf:jpegbuf, decompsrc? &srcsize:jpegsize,
					NULL, _w, _h, _subsamp, 0, filename, _tilew, _tileh)==-1)
				goto bailout;
		}
		else if(quiet==1) printf("N/A\n");

		for(i=0; i<ntilesw*ntilesh; i++)
		{
			tjFree(jpegbuf[i]);  jpegbuf[i]=NULL;
		}
		free(jpegbuf);  jpegbuf=NULL;
		if(jpegsize) {free(jpegsize);  jpegsize=NULL;}

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
	if(jpegsize) {free(jpegsize);  jpegsize=NULL;}
	if(srcbuf) {free(srcbuf);  srcbuf=NULL;}
	if(t) {free(t);  t=NULL;}
	if(handle) {tjDestroy(handle);  handle=NULL;}
	return retval;
}