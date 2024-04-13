int cmpBitmap(unsigned char *buf, int width, int pitch, int height, int pf,
	int flags, int gray2rgb)
{
	int roffset=tjRedOffset[pf];
	int goffset=tjGreenOffset[pf];
	int boffset=tjBlueOffset[pf];
	int aoffset=alphaOffset[pf];
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
			if(pf==TJPF_GRAY)
			{
				if(buf[row*pitch+i*ps]!=RGB2GRAY(r, g, b))
					return 0;
			}
			else if(pf==TJPF_CMYK)
			{
				unsigned char rf, gf, bf;
				cmyk_to_rgb(buf[row*pitch+i*ps+0], buf[row*pitch+i*ps+1],
					buf[row*pitch+i*ps+2], buf[row*pitch+i*ps+3], &rf, &gf,
					&bf);
				if(gray2rgb)
				{
					unsigned char gray=RGB2GRAY(r, g, b);
					if(rf!=gray || gf!=gray || bf!=gray)
						return 0;
				}
				else if(rf!=r || gf!=g || bf!=b) return 0;
			}
			else
			{
				if(gray2rgb)
				{
					unsigned char gray=RGB2GRAY(r, g, b);
					if(buf[row*pitch+i*ps+roffset]!=gray ||
						buf[row*pitch+i*ps+goffset]!=gray ||
						buf[row*pitch+i*ps+boffset]!=gray)
						return 0;
				}
				else if(buf[row*pitch+i*ps+roffset]!=r ||
					buf[row*pitch+i*ps+goffset]!=g ||
					buf[row*pitch+i*ps+boffset]!=b)
					return 0;
				if(aoffset>=0 && buf[row*pitch+i*ps+aoffset]!=0xFF)
					return 0;
			}
		}
	}
	return 1;
}