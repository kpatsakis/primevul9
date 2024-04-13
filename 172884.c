DLLEXPORT int DLLCALL tjSaveImage(const char *filename, unsigned char *buffer,
	int width, int pitch, int height, int pixelFormat, int flags)
{
	int retval=0;
	tjhandle handle=NULL;
	tjinstance *this;
	j_decompress_ptr dinfo=NULL;
	djpeg_dest_ptr dst;
	FILE *file=NULL;
	char *ptr=NULL;
	boolean invert;

	if(!filename || !buffer || width<1 || pitch<0 || height<1 || pixelFormat<0 ||
		pixelFormat>=TJ_NUMPF)
		_throwg("tjSaveImage(): Invalid argument");

	if((handle=tjInitDecompress())==NULL)
		return -1;
	this=(tjinstance *)handle;
	dinfo=&this->dinfo;

	if((file=fopen(filename, "wb"))==NULL)
		_throwunix("tjSaveImage(): Cannot open output file");

	if(setjmp(this->jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error. */
		retval=-1;  goto bailout;
	}

	this->dinfo.out_color_space=pf2cs[pixelFormat];
	dinfo->image_width=width;  dinfo->image_height=height;
	dinfo->global_state=DSTATE_READY;
	dinfo->scale_num=dinfo->scale_denom=1;

	ptr=strrchr(filename, '.');
	if(ptr && !strcasecmp(ptr, ".bmp"))
	{
		if((dst=jinit_write_bmp(dinfo, FALSE, FALSE))==NULL)
			_throwg("tjSaveImage(): Could not initialize bitmap writer");
		invert=(flags&TJFLAG_BOTTOMUP)==0;
	}
	else
	{
		if((dst=jinit_write_ppm(dinfo))==NULL)
			_throwg("tjSaveImage(): Could not initialize PPM writer");
		invert=(flags&TJFLAG_BOTTOMUP)!=0;
	}

	dst->output_file=file;
	(*dst->start_output)(dinfo, dst);
	(*dinfo->mem->realize_virt_arrays)((j_common_ptr)dinfo);

	if(pitch==0) pitch=width*tjPixelSize[pixelFormat];

	while(dinfo->output_scanline<dinfo->output_height)
	{
		unsigned char *rowptr;
		if(invert) rowptr=&buffer[(height-dinfo->output_scanline-1)*pitch];
		else rowptr=&buffer[dinfo->output_scanline*pitch];
		memcpy(dst->buffer[0], rowptr, width*tjPixelSize[pixelFormat]);
		(*dst->put_pixel_rows)(dinfo, dst, 1);
		dinfo->output_scanline++;
	}

	(*dst->finish_output)(dinfo, dst);

	bailout:
	if(handle) tjDestroy(handle);
	if(file) fclose(file);
	return retval;
}