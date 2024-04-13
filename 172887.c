int checkBufYUV(unsigned char *buf, int w, int h, int subsamp,
	tjscalingfactor sf)
{
	int row, col;
	int hsf=tjMCUWidth[subsamp]/8, vsf=tjMCUHeight[subsamp]/8;
	int pw=PAD(w, hsf), ph=PAD(h, vsf);
	int cw=pw/hsf, ch=ph/vsf;
	int ypitch=PAD(pw, pad), uvpitch=PAD(cw, pad);
	int retval=1;
	int halfway=16*sf.num/sf.denom;
	int blocksize=8*sf.num/sf.denom;

	for(row=0; row<ph; row++)
	{
		for(col=0; col<pw; col++)
		{
			unsigned char y=buf[ypitch*row+col];
			if(((row/blocksize)+(col/blocksize))%2==0)
			{
				if(row<halfway) checkval255(y)  else checkval0(y);
			}
			else
			{
				if(row<halfway) checkval(y, 76)  else checkval(y, 226);
			}
		}
	}
	if(subsamp!=TJSAMP_GRAY)
	{
		int halfway=16/vsf*sf.num/sf.denom;
		for(row=0; row<ch; row++)
		{
			for(col=0; col<cw; col++)
			{
				unsigned char u=buf[ypitch*ph + (uvpitch*row+col)],
					v=buf[ypitch*ph + uvpitch*ch + (uvpitch*row+col)];
				if(((row*vsf/blocksize)+(col*hsf/blocksize))%2==0)
				{
					checkval(u, 128);  checkval(v, 128);
				}
				else
				{
					if(row<halfway)
					{
						checkval(u, 85);  checkval255(v);
					}
					else
					{
						checkval0(u);  checkval(v, 149);
					}
				}
			}
		}
	}

	bailout:
	if(retval==0)
	{
		for(row=0; row<ph; row++)
		{
			for(col=0; col<pw; col++)
				printf("%.3d ", buf[ypitch*row+col]);
			printf("\n");
		}
		printf("\n");
		for(row=0; row<ch; row++)
		{
			for(col=0; col<cw; col++)
				printf("%.3d ", buf[ypitch*ph + (uvpitch*row+col)]);
			printf("\n");
		}
		printf("\n");
		for(row=0; row<ch; row++)
		{
			for(col=0; col<cw; col++)
				printf("%.3d ", buf[ypitch*ph + uvpitch*ch + (uvpitch*row+col)]);
			printf("\n");
		}
	}

	return retval;
}