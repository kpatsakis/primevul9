int checkBuf(unsigned char *buf, int w, int h, int pf, int subsamp,
	tjscalingfactor sf, int flags)
{
	int roffset=tjRedOffset[pf];
	int goffset=tjGreenOffset[pf];
	int boffset=tjBlueOffset[pf];
	int aoffset=alphaOffset[pf];
	int ps=tjPixelSize[pf];
	int index, row, col, retval=1;
	int halfway=16*sf.num/sf.denom;
	int blocksize=8*sf.num/sf.denom;

	if(pf==TJPF_CMYK)
	{
		for(row=0; row<h; row++)
		{
			for(col=0; col<w; col++)
			{
				unsigned char c, m, y, k;
				if(flags&TJFLAG_BOTTOMUP) index=(h-row-1)*w+col;
				else index=row*w+col;
				c=buf[index*ps];
				m=buf[index*ps+1];
				y=buf[index*ps+2];
				k=buf[index*ps+3];
				if(((row/blocksize)+(col/blocksize))%2==0)
				{
					checkval255(c);  checkval255(m);  checkval255(y);
					if(row<halfway) checkval255(k)
					else checkval0(k)
				}
				else
				{
					checkval255(c);  checkval0(y);  checkval255(k);
					if(row<halfway) checkval0(m)
					else checkval255(m)
				}
			}
		}
		return 1;
	}

	for(row=0; row<h; row++)
	{
		for(col=0; col<w; col++)
		{
			unsigned char r, g, b, a;
			if(flags&TJFLAG_BOTTOMUP) index=(h-row-1)*w+col;
			else index=row*w+col;
			r=buf[index*ps+roffset];
			g=buf[index*ps+goffset];
			b=buf[index*ps+boffset];
			a=aoffset>=0? buf[index*ps+aoffset]:0xFF;
			if(((row/blocksize)+(col/blocksize))%2==0)
			{
				if(row<halfway)
				{
					checkval255(r);  checkval255(g);  checkval255(b);
				}
				else
				{
					checkval0(r);  checkval0(g);  checkval0(b);
				}
			}
			else
			{
				if(subsamp==TJSAMP_GRAY)
				{
					if(row<halfway)
					{
						checkval(r, 76);  checkval(g, 76);  checkval(b, 76);
					}
					else
					{
						checkval(r, 226);  checkval(g, 226);  checkval(b, 226);
					}
				}
				else
				{
					if(row<halfway)
					{
						checkval255(r);  checkval0(g);  checkval0(b);
					}
					else
					{
						checkval255(r);  checkval255(g);  checkval0(b);
					}
				}
			}
			checkval255(a);
		}
	}

	bailout:
	if(retval==0)
	{
		for(row=0; row<h; row++)
		{
			for(col=0; col<w; col++)
			{
				if(pf==TJPF_CMYK)
					printf("%.3d/%.3d/%.3d/%.3d ", buf[(row*w+col)*ps],
						buf[(row*w+col)*ps+1], buf[(row*w+col)*ps+2],
						buf[(row*w+col)*ps+3]);
				else
					printf("%.3d/%.3d/%.3d ", buf[(row*w+col)*ps+roffset],
						buf[(row*w+col)*ps+goffset], buf[(row*w+col)*ps+boffset]);
			}
			printf("\n");
		}
	}
	return retval;
}