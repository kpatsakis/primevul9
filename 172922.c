void initBuf(unsigned char *buf, int w, int h, int pf, int flags)
{
	int roffset=tjRedOffset[pf];
	int goffset=tjGreenOffset[pf];
	int boffset=tjBlueOffset[pf];
	int ps=tjPixelSize[pf];
	int index, row, col, halfway=16;

	if(pf==TJPF_GRAY)
	{
		memset(buf, 0, w*h*ps);
		for(row=0; row<h; row++)
		{
			for(col=0; col<w; col++)
			{
				if(flags&TJFLAG_BOTTOMUP) index=(h-row-1)*w+col;
				else index=row*w+col;
				if(((row/8)+(col/8))%2==0) buf[index]=(row<halfway)? 255:0;
				else buf[index]=(row<halfway)? 76:226;
			}
		}
	}
	else if(pf==TJPF_CMYK)
	{
		memset(buf, 255, w*h*ps);
		for(row=0; row<h; row++)
		{
			for(col=0; col<w; col++)
			{
				if(flags&TJFLAG_BOTTOMUP) index=(h-row-1)*w+col;
				else index=row*w+col;
				if(((row/8)+(col/8))%2==0)
				{
					if(row>=halfway) buf[index*ps+3]=0;
				}
				else
				{
					buf[index*ps+2]=0;
					if(row<halfway) buf[index*ps+1]=0;
				}
			}
		}
	}
	else
	{
		memset(buf, 0, w*h*ps);
		for(row=0; row<h; row++)
		{
			for(col=0; col<w; col++)
			{
				if(flags&TJFLAG_BOTTOMUP) index=(h-row-1)*w+col;
				else index=row*w+col;
				if(((row/8)+(col/8))%2==0)
				{
					if(row<halfway)
					{
						buf[index*ps+roffset]=255;
						buf[index*ps+goffset]=255;
						buf[index*ps+boffset]=255;
					}
				}
				else
				{
					buf[index*ps+roffset]=255;
					if(row>=halfway) buf[index*ps+goffset]=255;
				}
			}
		}
	}
}