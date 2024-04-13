DLLEXPORT int DLLCALL tjDecompressToYUVPlanes(tjhandle handle,
	const unsigned char *jpegBuf, unsigned long jpegSize,
	unsigned char **dstPlanes, int width, int *strides, int height, int flags)
{
	int i, sfi, row, retval=0;  JSAMPROW *outbuf[MAX_COMPONENTS];
	int jpegwidth, jpegheight, jpegSubsamp, scaledw, scaledh;
	int pw[MAX_COMPONENTS], ph[MAX_COMPONENTS], iw[MAX_COMPONENTS],
		tmpbufsize=0, usetmpbuf=0, th[MAX_COMPONENTS];
	JSAMPLE *_tmpbuf=NULL, *ptr;  JSAMPROW *tmpbuf[MAX_COMPONENTS];
	int dctsize;

	getdinstance(handle);
	this->jerr.stopOnWarning=(flags & TJFLAG_STOPONWARNING) ? TRUE : FALSE;

	for(i=0; i<MAX_COMPONENTS; i++)
	{
		tmpbuf[i]=NULL;  outbuf[i]=NULL;
	}

	if((this->init&DECOMPRESS)==0)
		_throw("tjDecompressToYUVPlanes(): Instance has not been initialized for decompression");

	if(jpegBuf==NULL || jpegSize<=0 || !dstPlanes || !dstPlanes[0] || width<0
		|| height<0)
		_throw("tjDecompressToYUVPlanes(): Invalid argument");

	if(flags&TJFLAG_FORCEMMX) putenv("JSIMD_FORCEMMX=1");
	else if(flags&TJFLAG_FORCESSE) putenv("JSIMD_FORCESSE=1");
	else if(flags&TJFLAG_FORCESSE2) putenv("JSIMD_FORCESSE2=1");

	if(setjmp(this->jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error. */
		retval=-1;  goto bailout;
	}

	if(!this->headerRead)
	{
		jpeg_mem_src_tj(dinfo, jpegBuf, jpegSize);
		jpeg_read_header(dinfo, TRUE);
	}
	this->headerRead=0;
	jpegSubsamp=getSubsamp(dinfo);
	if(jpegSubsamp<0)
		_throw("tjDecompressToYUVPlanes(): Could not determine subsampling type for JPEG image");

	if(jpegSubsamp!=TJSAMP_GRAY && (!dstPlanes[1] || !dstPlanes[2]))
		_throw("tjDecompressToYUVPlanes(): Invalid argument");

	jpegwidth=dinfo->image_width;  jpegheight=dinfo->image_height;
	if(width==0) width=jpegwidth;
	if(height==0) height=jpegheight;
	for(i=0; i<NUMSF; i++)
	{
		scaledw=TJSCALED(jpegwidth, sf[i]);
		scaledh=TJSCALED(jpegheight, sf[i]);
		if(scaledw<=width && scaledh<=height)
			break;
	}
	if(i>=NUMSF)
		_throw("tjDecompressToYUVPlanes(): Could not scale down to desired image dimensions");
	if(dinfo->num_components>3)
		_throw("tjDecompressToYUVPlanes(): JPEG image must have 3 or fewer components");

	width=scaledw;  height=scaledh;
	dinfo->scale_num=sf[i].num;
	dinfo->scale_denom=sf[i].denom;
	sfi=i;
	jpeg_calc_output_dimensions(dinfo);

	dctsize=DCTSIZE*sf[sfi].num/sf[sfi].denom;

	for(i=0; i<dinfo->num_components; i++)
	{
		jpeg_component_info *compptr=&dinfo->comp_info[i];
		int ih;
		iw[i]=compptr->width_in_blocks*dctsize;
		ih=compptr->height_in_blocks*dctsize;
		pw[i]=PAD(dinfo->output_width, dinfo->max_h_samp_factor)
			*compptr->h_samp_factor/dinfo->max_h_samp_factor;
		ph[i]=PAD(dinfo->output_height, dinfo->max_v_samp_factor)
			*compptr->v_samp_factor/dinfo->max_v_samp_factor;
		if(iw[i]!=pw[i] || ih!=ph[i]) usetmpbuf=1;
		th[i]=compptr->v_samp_factor*dctsize;
		tmpbufsize+=iw[i]*th[i];
		if((outbuf[i]=(JSAMPROW *)malloc(sizeof(JSAMPROW)*ph[i]))==NULL)
			_throw("tjDecompressToYUVPlanes(): Memory allocation failure");
		ptr=dstPlanes[i];
		for(row=0; row<ph[i]; row++)
		{
			outbuf[i][row]=ptr;
			ptr+=(strides && strides[i]!=0)? strides[i]:pw[i];
		}
	}
	if(usetmpbuf)
	{
		if((_tmpbuf=(JSAMPLE *)malloc(sizeof(JSAMPLE)*tmpbufsize))==NULL)
			_throw("tjDecompressToYUVPlanes(): Memory allocation failure");
		ptr=_tmpbuf;
		for(i=0; i<dinfo->num_components; i++)
		{
			if((tmpbuf[i]=(JSAMPROW *)malloc(sizeof(JSAMPROW)*th[i]))==NULL)
				_throw("tjDecompressToYUVPlanes(): Memory allocation failure");
			for(row=0; row<th[i]; row++)
			{
				tmpbuf[i][row]=ptr;
				ptr+=iw[i];
			}
		}
	}

	if(setjmp(this->jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error. */
		retval=-1;  goto bailout;
	}

	if(flags&TJFLAG_FASTUPSAMPLE) dinfo->do_fancy_upsampling=FALSE;
	if(flags&TJFLAG_FASTDCT) dinfo->dct_method=JDCT_FASTEST;
	dinfo->raw_data_out=TRUE;

	jpeg_start_decompress(dinfo);
	for(row=0; row<(int)dinfo->output_height;
		row+=dinfo->max_v_samp_factor*dinfo->_min_DCT_scaled_size)
	{
		JSAMPARRAY yuvptr[MAX_COMPONENTS];
		int crow[MAX_COMPONENTS];
		for(i=0; i<dinfo->num_components; i++)
		{
			jpeg_component_info *compptr=&dinfo->comp_info[i];
			if(jpegSubsamp==TJ_420)
			{
				/* When 4:2:0 subsampling is used with IDCT scaling, libjpeg will try
				   to be clever and use the IDCT to perform upsampling on the U and V
				   planes.  For instance, if the output image is to be scaled by 1/2
				   relative to the JPEG image, then the scaling factor and upsampling
				   effectively cancel each other, so a normal 8x8 IDCT can be used.
				   However, this is not desirable when using the decompress-to-YUV
				   functionality in TurboJPEG, since we want to output the U and V
				   planes in their subsampled form.  Thus, we have to override some
				   internal libjpeg parameters to force it to use the "scaled" IDCT
				   functions on the U and V planes. */
				compptr->_DCT_scaled_size=dctsize;
				compptr->MCU_sample_width=tjMCUWidth[jpegSubsamp]*
					sf[sfi].num/sf[sfi].denom*
					compptr->v_samp_factor/dinfo->max_v_samp_factor;
				dinfo->idct->inverse_DCT[i] = dinfo->idct->inverse_DCT[0];
			}
			crow[i]=row*compptr->v_samp_factor/dinfo->max_v_samp_factor;
			if(usetmpbuf) yuvptr[i]=tmpbuf[i];
			else yuvptr[i]=&outbuf[i][crow[i]];
		}
		jpeg_read_raw_data(dinfo, yuvptr,
			dinfo->max_v_samp_factor*dinfo->_min_DCT_scaled_size);
		if(usetmpbuf)
		{
			int j;
			for(i=0; i<dinfo->num_components; i++)
			{
				for(j=0; j<min(th[i], ph[i]-crow[i]); j++)
				{
					memcpy(outbuf[i][crow[i]+j], tmpbuf[i][j], pw[i]);
				}
			}
		}
	}
	jpeg_finish_decompress(dinfo);

	bailout:
	if(dinfo->global_state>DSTATE_START) jpeg_abort_decompress(dinfo);
	for(i=0; i<MAX_COMPONENTS; i++)
	{
		if(tmpbuf[i]) free(tmpbuf[i]);
		if(outbuf[i]) free(outbuf[i]);
	}
	if(_tmpbuf) free(_tmpbuf);
	if(this->jerr.warning) retval=-1;
	this->jerr.stopOnWarning=FALSE;
	return retval;
}