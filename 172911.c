int main(int argc, char *argv[])
{
	unsigned char *srcbuf=NULL;  int w=0, h=0, i, j;
	int minqual=-1, maxqual=-1;  char *temp;
	int minarg=2, retval=0, subsamp=-1;

	if((scalingfactors=tjGetScalingFactors(&nsf))==NULL || nsf==0)
		_throw("executing tjGetScalingFactors()", tjGetErrorStr());

	if(argc<minarg) usage(argv[0]);

	temp=strrchr(argv[1], '.');
	if(temp!=NULL)
	{
		if(!strcasecmp(temp, ".bmp")) ext="bmp";
		if(!strcasecmp(temp, ".jpg") || !strcasecmp(temp, ".jpeg")) decomponly=1;
	}

	printf("\n");

	if(!decomponly)
	{
		minarg=3;
		if(argc<minarg) usage(argv[0]);
		if((minqual=atoi(argv[2]))<1 || minqual>100)
		{
			puts("ERROR: Quality must be between 1 and 100.");
			exit(1);
		}
		if((temp=strchr(argv[2], '-'))!=NULL && strlen(temp)>1
			&& sscanf(&temp[1], "%d", &maxqual)==1 && maxqual>minqual && maxqual>=1
			&& maxqual<=100) {}
		else maxqual=minqual;
	}

	if(argc>minarg)
	{
		for(i=minarg; i<argc; i++)
		{
			if(!strcasecmp(argv[i], "-tile"))
			{
				dotile=1;  xformopt|=TJXOPT_CROP;
			}
			else if(!strcasecmp(argv[i], "-fastupsample"))
			{
				printf("Using fast upsampling code\n\n");
				flags|=TJFLAG_FASTUPSAMPLE;
			}
			else if(!strcasecmp(argv[i], "-fastdct"))
			{
				printf("Using fastest DCT/IDCT algorithm\n\n");
				flags|=TJFLAG_FASTDCT;
			}
			else if(!strcasecmp(argv[i], "-accuratedct"))
			{
				printf("Using most accurate DCT/IDCT algorithm\n\n");
				flags|=TJFLAG_ACCURATEDCT;
			}
			else if(!strcasecmp(argv[i], "-progressive"))
			{
				printf("Using progressive entropy coding\n\n");
				flags|=TJFLAG_PROGRESSIVE;
			}
			else if(!strcasecmp(argv[i], "-rgb")) pf=TJPF_RGB;
			else if(!strcasecmp(argv[i], "-rgbx")) pf=TJPF_RGBX;
			else if(!strcasecmp(argv[i], "-bgr")) pf=TJPF_BGR;
			else if(!strcasecmp(argv[i], "-bgrx")) pf=TJPF_BGRX;
			else if(!strcasecmp(argv[i], "-xbgr")) pf=TJPF_XBGR;
			else if(!strcasecmp(argv[i], "-xrgb")) pf=TJPF_XRGB;
			else if(!strcasecmp(argv[i], "-cmyk")) pf=TJPF_CMYK;
			else if(!strcasecmp(argv[i], "-bottomup")) flags|=TJFLAG_BOTTOMUP;
			else if(!strcasecmp(argv[i], "-quiet")) quiet=1;
			else if(!strcasecmp(argv[i], "-qq")) quiet=2;
			else if(!strcasecmp(argv[i], "-scale") && i<argc-1)
			{
				int temp1=0, temp2=0, match=0;
				if(sscanf(argv[++i], "%d/%d", &temp1, &temp2)==2)
				{
					for(j=0; j<nsf; j++)
					{
						if((double)temp1/(double)temp2
							== (double)scalingfactors[j].num/(double)scalingfactors[j].denom)
						{
							sf=scalingfactors[j];
							match=1;  break;
						}
					}
					if(!match) usage(argv[0]);
				}
				else usage(argv[0]);
			}
			else if(!strcasecmp(argv[i], "-hflip")) xformop=TJXOP_HFLIP;
			else if(!strcasecmp(argv[i], "-vflip")) xformop=TJXOP_VFLIP;
			else if(!strcasecmp(argv[i], "-transpose")) xformop=TJXOP_TRANSPOSE;
			else if(!strcasecmp(argv[i], "-transverse")) xformop=TJXOP_TRANSVERSE;
			else if(!strcasecmp(argv[i], "-rot90")) xformop=TJXOP_ROT90;
			else if(!strcasecmp(argv[i], "-rot180")) xformop=TJXOP_ROT180;
			else if(!strcasecmp(argv[i], "-rot270")) xformop=TJXOP_ROT270;
			else if(!strcasecmp(argv[i], "-grayscale")) xformopt|=TJXOPT_GRAY;
			else if(!strcasecmp(argv[i], "-custom")) customFilter=dummyDCTFilter;
			else if(!strcasecmp(argv[i], "-nooutput")) xformopt|=TJXOPT_NOOUTPUT;
			else if(!strcasecmp(argv[i], "-copynone")) xformopt|=TJXOPT_COPYNONE;
			else if(!strcasecmp(argv[i], "-benchtime") && i<argc-1)
			{
				double temp=atof(argv[++i]);
				if(temp>0.0) benchtime=temp;
				else usage(argv[0]);
			}
			else if(!strcasecmp(argv[i], "-warmup") && i<argc-1)
			{
				double temp=atof(argv[++i]);
				if(temp>=0.0) warmup=temp;
				else usage(argv[0]);
				printf("Warmup time = %.1f seconds\n\n", warmup);
			}
			else if(!strcasecmp(argv[i], "-alloc")) flags&=(~TJFLAG_NOREALLOC);
			else if(!strcasecmp(argv[i], "-bmp")) ext="bmp";
			else if(!strcasecmp(argv[i], "-yuv"))
			{
				printf("Testing YUV planar encoding/decoding\n\n");
				doyuv=1;
			}
			else if(!strcasecmp(argv[i], "-yuvpad") && i<argc-1)
			{
				int temp=atoi(argv[++i]);
				if(temp>=1) yuvpad=temp;
			}
			else if(!strcasecmp(argv[i], "-subsamp") && i<argc-1)
			{
				i++;
				if(toupper(argv[i][0])=='G') subsamp=TJSAMP_GRAY;
				else
				{
					int temp=atoi(argv[i]);
					switch(temp)
					{
						case 444:  subsamp=TJSAMP_444;  break;
						case 422:  subsamp=TJSAMP_422;  break;
						case 440:  subsamp=TJSAMP_440;  break;
						case 420:  subsamp=TJSAMP_420;  break;
						case 411:  subsamp=TJSAMP_411;  break;
					}
				}
			}
			else if(!strcasecmp(argv[i], "-componly")) componly=1;
			else if(!strcasecmp(argv[i], "-nowrite")) dowrite=0;
			else if(!strcasecmp(argv[i], "-stoponwarning")) flags|=TJFLAG_STOPONWARNING;
			else usage(argv[0]);
		}
	}

	if((sf.num!=1 || sf.denom!=1) && dotile)
	{
		printf("Disabling tiled compression/decompression tests, because those tests do not\n");
		printf("work when scaled decompression is enabled.\n");
		dotile=0;
	}

	if((flags&TJFLAG_NOREALLOC)==0 && dotile)
	{
		printf("Disabling tiled compression/decompression tests, because those tests do not\n");
		printf("work when dynamic JPEG buffer allocation is enabled.\n\n");
		dotile=0;
	}

	if(!decomponly)
	{
		if((srcbuf=tjLoadImage(argv[1], &w, 1, &h, &pf, flags))==NULL)
			_throwtjg("loading bitmap");
		temp=strrchr(argv[1], '.');
		if(temp!=NULL) *temp='\0';
	}

	if(quiet==1 && !decomponly)
	{
		printf("All performance values in Mpixels/sec\n\n");
		printf("Bitmap     JPEG     JPEG  %s  %s   ",
			dotile? "Tile ":"Image", dotile? "Tile ":"Image");
		if(doyuv) printf("Encode  ");
		printf("Comp    Comp    Decomp  ");
		if(doyuv) printf("Decode");
		printf("\n");
		printf("Format     Subsamp  Qual  Width  Height  ");
		if(doyuv) printf("Perf    ");
		printf("Perf    Ratio   Perf    ");
		if(doyuv) printf("Perf");
		printf("\n\n");
	}

	if(decomponly)
	{
		decompTest(argv[1]);
		printf("\n");
		goto bailout;
	}
	if(subsamp>=0 && subsamp<TJ_NUMSAMP)
	{
		for(i=maxqual; i>=minqual; i--)
			fullTest(srcbuf, w, h, subsamp, i, argv[1]);
		printf("\n");
	}
	else
	{
		if(pf!=TJPF_CMYK)
		{
			for(i=maxqual; i>=minqual; i--)
				fullTest(srcbuf, w, h, TJSAMP_GRAY, i, argv[1]);
			printf("\n");
		}
		for(i=maxqual; i>=minqual; i--)
			fullTest(srcbuf, w, h, TJSAMP_420, i, argv[1]);
		printf("\n");
		for(i=maxqual; i>=minqual; i--)
			fullTest(srcbuf, w, h, TJSAMP_422, i, argv[1]);
		printf("\n");
		for(i=maxqual; i>=minqual; i--)
			fullTest(srcbuf, w, h, TJSAMP_444, i, argv[1]);
		printf("\n");
	}

	bailout:
	if(srcbuf) tjFree(srcbuf);
	return retval;
}