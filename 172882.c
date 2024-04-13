DLLEXPORT int DLLCALL tjCompressFromYUVPlanes(tjhandle handle,
	const unsigned char **srcPlanes, int width, const int *strides, int height,
	int subsamp, unsigned char **jpegBuf, unsigned long *jpegSize, int jpegQual,
	int flags)
{
	int i, row, retval=0, alloc=1;  JSAMPROW *inbuf[MAX_COMPONENTS];
	int pw[MAX_COMPONENTS], ph[MAX_COMPONENTS], iw[MAX_COMPONENTS],
		tmpbufsize=0, usetmpbuf=0, th[MAX_COMPONENTS];
	JSAMPLE *_tmpbuf=NULL, *ptr;  JSAMPROW *tmpbuf[MAX_COMPONENTS];

	getcinstance(handle)
	this->jerr.stopOnWarning=(flags & TJFLAG_STOPONWARNING) ? TRUE : FALSE;

	for(i=0; i<MAX_COMPONENTS; i++)
	{
		tmpbuf[i]=NULL;  inbuf[i]=NULL;
	}

	if((this->init&COMPRESS)==0)
		_throw("tjCompressFromYUVPlanes(): Instance has not been initialized for compression");

	if(!srcPlanes || !srcPlanes[0] || width<=0 || height<=0 || subsamp<0
		|| subsamp>=NUMSUBOPT || jpegBuf==NULL || jpegSize==NULL || jpegQual<0
		|| jpegQual>100)
		_throw("tjCompressFromYUVPlanes(): Invalid argument");
	if(subsamp!=TJSAMP_GRAY && (!srcPlanes[1] || !srcPlanes[2]))
		_throw("tjCompressFromYUVPlanes(): Invalid argument");

	if(setjmp(this->jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error. */
		retval=-1;  goto bailout;
	}

	cinfo->image_width=width;
	cinfo->image_height=height;

	if(flags&TJFLAG_FORCEMMX) putenv("JSIMD_FORCEMMX=1");
	else if(flags&TJFLAG_FORCESSE) putenv("JSIMD_FORCESSE=1");
	else if(flags&TJFLAG_FORCESSE2) putenv("JSIMD_FORCESSE2=1");

	if(flags&TJFLAG_NOREALLOC)
	{
		alloc=0;  *jpegSize=tjBufSize(width, height, subsamp);
	}
	jpeg_mem_dest_tj(cinfo, jpegBuf, jpegSize, alloc);
	if(setCompDefaults(cinfo, TJPF_RGB, subsamp, jpegQual, flags)==-1)
		return -1;
	cinfo->raw_data_in=TRUE;

	jpeg_start_compress(cinfo, TRUE);
	for(i=0; i<cinfo->num_components; i++)
	{
		jpeg_component_info *compptr=&cinfo->comp_info[i];
		int ih;
		iw[i]=compptr->width_in_blocks*DCTSIZE;
		ih=compptr->height_in_blocks*DCTSIZE;
		pw[i]=PAD(cinfo->image_width, cinfo->max_h_samp_factor)
			*compptr->h_samp_factor/cinfo->max_h_samp_factor;
		ph[i]=PAD(cinfo->image_height, cinfo->max_v_samp_factor)
			*compptr->v_samp_factor/cinfo->max_v_samp_factor;
		if(iw[i]!=pw[i] || ih!=ph[i]) usetmpbuf=1;
		th[i]=compptr->v_samp_factor*DCTSIZE;
		tmpbufsize+=iw[i]*th[i];
		if((inbuf[i]=(JSAMPROW *)malloc(sizeof(JSAMPROW)*ph[i]))==NULL)
			_throw("tjCompressFromYUVPlanes(): Memory allocation failure");
		ptr=(JSAMPLE *)srcPlanes[i];
		for(row=0; row<ph[i]; row++)
		{
			inbuf[i][row]=ptr;
			ptr+=(strides && strides[i]!=0)? strides[i]:pw[i];
		}
	}
	if(usetmpbuf)
	{
		if((_tmpbuf=(JSAMPLE *)malloc(sizeof(JSAMPLE)*tmpbufsize))==NULL)
			_throw("tjCompressFromYUVPlanes(): Memory allocation failure");
		ptr=_tmpbuf;
		for(i=0; i<cinfo->num_components; i++)
		{
			if((tmpbuf[i]=(JSAMPROW *)malloc(sizeof(JSAMPROW)*th[i]))==NULL)
				_throw("tjCompressFromYUVPlanes(): Memory allocation failure");
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

	for(row=0; row<(int)cinfo->image_height;
		row+=cinfo->max_v_samp_factor*DCTSIZE)
	{
		JSAMPARRAY yuvptr[MAX_COMPONENTS];
		int crow[MAX_COMPONENTS];
		for(i=0; i<cinfo->num_components; i++)
		{
			jpeg_component_info *compptr=&cinfo->comp_info[i];
			crow[i]=row*compptr->v_samp_factor/cinfo->max_v_samp_factor;
			if(usetmpbuf)
			{
				int j, k;
				for(j=0; j<min(th[i], ph[i]-crow[i]); j++)
				{
					memcpy(tmpbuf[i][j], inbuf[i][crow[i]+j], pw[i]);
					/* Duplicate last sample in row to fill out MCU */
					for(k=pw[i]; k<iw[i]; k++) tmpbuf[i][j][k]=tmpbuf[i][j][pw[i]-1];
				}
				/* Duplicate last row to fill out MCU */
				for(j=ph[i]-crow[i]; j<th[i]; j++)
					memcpy(tmpbuf[i][j], tmpbuf[i][ph[i]-crow[i]-1], iw[i]);
				yuvptr[i]=tmpbuf[i];
			}
			else
				yuvptr[i]=&inbuf[i][crow[i]];
		}
		jpeg_write_raw_data(cinfo, yuvptr, cinfo->max_v_samp_factor*DCTSIZE);
	}
	jpeg_finish_compress(cinfo);

	bailout:
	if(cinfo->global_state>CSTATE_START) jpeg_abort_compress(cinfo);
	for(i=0; i<MAX_COMPONENTS; i++)
	{
		if(tmpbuf[i]) free(tmpbuf[i]);
		if(inbuf[i]) free(inbuf[i]);
	}
	if(_tmpbuf) free(_tmpbuf);
	if(this->jerr.warning) retval=-1;
	this->jerr.stopOnWarning=FALSE;
	return retval;
}