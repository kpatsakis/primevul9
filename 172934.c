void initBitmap(unsigned char *buf, int width, int pitch, int height, int pf,
	int flags)
{
	int roffset=tjRedOffset[pf];
	int goffset=tjGreenOffset[pf];
	int boffset=tjBlueOffset[pf];
	int ps=tjPixelSize[pf];
	int i, j;

	for(j=0; j<height; j++)
	{
		int row=(flags&TJFLAG_BOTTOMUP)? height-j-1:j;
		for(i=0; i<width; i++)
		{
			unsigned char r=(i*256/width)%256;
			unsigned char g=(j*256/height)%256;
			unsigned char b=(j*256/height+i*256/width)%256;
			memset(&buf[row*pitch+i*ps], 0, ps);
			if(pf==TJPF_GRAY) buf[row*pitch+i*ps]=RGB2GRAY(r, g, b);
			else if(pf==TJPF_CMYK)
				rgb_to_cmyk(r, g, b, &buf[row*pitch+i*ps+0], &buf[row*pitch+i*ps+1],
					&buf[row*pitch+i*ps+2], &buf[row*pitch+i*ps+3]);
			else
			{
				buf[row*pitch+i*ps+roffset]=r;
				buf[row*pitch+i*ps+goffset]=g;
				buf[row*pitch+i*ps+boffset]=b;
			}
		}
	}
}