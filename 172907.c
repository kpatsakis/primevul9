DLLEXPORT int DLLCALL tjTransform(tjhandle handle,
	const unsigned char *jpegBuf, unsigned long jpegSize, int n,
	unsigned char **dstBufs, unsigned long *dstSizes, tjtransform *t, int flags)
{
	jpeg_transform_info *xinfo=NULL;
	jvirt_barray_ptr *srccoefs, *dstcoefs;
	int retval=0, i, jpegSubsamp, saveMarkers=0;

	getinstance(handle);
	this->jerr.stopOnWarning=(flags & TJFLAG_STOPONWARNING) ? TRUE : FALSE;
	if((this->init&COMPRESS)==0 || (this->init&DECOMPRESS)==0)
		_throw("tjTransform(): Instance has not been initialized for transformation");

	if(jpegBuf==NULL || jpegSize<=0 || n<1 || dstBufs==NULL || dstSizes==NULL
		|| t==NULL || flags<0)
		_throw("tjTransform(): Invalid argument");

	if(flags&TJFLAG_FORCEMMX) putenv("JSIMD_FORCEMMX=1");
	else if(flags&TJFLAG_FORCESSE) putenv("JSIMD_FORCESSE=1");
	else if(flags&TJFLAG_FORCESSE2) putenv("JSIMD_FORCESSE2=1");

	if((xinfo=(jpeg_transform_info *)malloc(sizeof(jpeg_transform_info)*n))
		==NULL)
		_throw("tjTransform(): Memory allocation failure");
	MEMZERO(xinfo, sizeof(jpeg_transform_info)*n);

	if(setjmp(this->jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error. */
		retval=-1;  goto bailout;
	}

	jpeg_mem_src_tj(dinfo, jpegBuf, jpegSize);

	for(i=0; i<n; i++)
	{
		xinfo[i].transform=xformtypes[t[i].op];
		xinfo[i].perfect=(t[i].options&TJXOPT_PERFECT)? 1:0;
		xinfo[i].trim=(t[i].options&TJXOPT_TRIM)? 1:0;
		xinfo[i].force_grayscale=(t[i].options&TJXOPT_GRAY)? 1:0;
		xinfo[i].crop=(t[i].options&TJXOPT_CROP)? 1:0;
		if(n!=1 && t[i].op==TJXOP_HFLIP) xinfo[i].slow_hflip=1;
		else xinfo[i].slow_hflip=0;

		if(xinfo[i].crop)
		{
			xinfo[i].crop_xoffset=t[i].r.x;  xinfo[i].crop_xoffset_set=JCROP_POS;
			xinfo[i].crop_yoffset=t[i].r.y;  xinfo[i].crop_yoffset_set=JCROP_POS;
			if(t[i].r.w!=0)
			{
				xinfo[i].crop_width=t[i].r.w;  xinfo[i].crop_width_set=JCROP_POS;
			}
			else xinfo[i].crop_width=JCROP_UNSET;
			if(t[i].r.h!=0)
			{
				xinfo[i].crop_height=t[i].r.h;  xinfo[i].crop_height_set=JCROP_POS;
			}
			else xinfo[i].crop_height=JCROP_UNSET;
		}
		if(!(t[i].options&TJXOPT_COPYNONE)) saveMarkers=1;
	}

	jcopy_markers_setup(dinfo, saveMarkers ? JCOPYOPT_ALL:JCOPYOPT_NONE);
	jpeg_read_header(dinfo, TRUE);
	jpegSubsamp=getSubsamp(dinfo);
	if(jpegSubsamp<0)
		_throw("tjTransform(): Could not determine subsampling type for JPEG image");

	for(i=0; i<n; i++)
	{
		if(!jtransform_request_workspace(dinfo, &xinfo[i]))
			_throw("tjTransform(): Transform is not perfect");

		if(xinfo[i].crop)
		{
			if((t[i].r.x%xinfo[i].iMCU_sample_width)!=0
				|| (t[i].r.y%xinfo[i].iMCU_sample_height)!=0)
			{
				snprintf(errStr, JMSG_LENGTH_MAX,
					"To crop this JPEG image, x must be a multiple of %d\n"
					"and y must be a multiple of %d.\n",
					xinfo[i].iMCU_sample_width, xinfo[i].iMCU_sample_height);
				retval=-1;  goto bailout;
			}
		}
	}

	srccoefs=jpeg_read_coefficients(dinfo);

	for(i=0; i<n; i++)
	{
		int w, h, alloc=1;
		if(!xinfo[i].crop)
		{
			w=dinfo->image_width;  h=dinfo->image_height;
		}
		else
		{
			w=xinfo[i].crop_width;  h=xinfo[i].crop_height;
		}
		if(flags&TJFLAG_NOREALLOC)
		{
			alloc=0;  dstSizes[i]=tjBufSize(w, h, jpegSubsamp);
		}
		if(!(t[i].options&TJXOPT_NOOUTPUT))
			jpeg_mem_dest_tj(cinfo, &dstBufs[i], &dstSizes[i], alloc);
		jpeg_copy_critical_parameters(dinfo, cinfo);
		dstcoefs=jtransform_adjust_parameters(dinfo, cinfo, srccoefs,
			&xinfo[i]);
		if(flags&TJFLAG_PROGRESSIVE || t[i].options&TJXOPT_PROGRESSIVE)
			jpeg_simple_progression(cinfo);
		if(!(t[i].options&TJXOPT_NOOUTPUT))
		{
			jpeg_write_coefficients(cinfo, dstcoefs);
			jcopy_markers_execute(dinfo, cinfo,
				t[i].options&TJXOPT_COPYNONE ? JCOPYOPT_NONE:JCOPYOPT_ALL);
		}
		else jinit_c_master_control(cinfo, TRUE);
		jtransform_execute_transformation(dinfo, cinfo, srccoefs,
			&xinfo[i]);
		if(t[i].customFilter)
		{
			int ci, y;  JDIMENSION by;
			for(ci=0; ci<cinfo->num_components; ci++)
			{
				jpeg_component_info *compptr=&cinfo->comp_info[ci];
				tjregion arrayRegion={0, 0, compptr->width_in_blocks*DCTSIZE,
					DCTSIZE};
				tjregion planeRegion={0, 0, compptr->width_in_blocks*DCTSIZE,
					compptr->height_in_blocks*DCTSIZE};
				for(by=0; by<compptr->height_in_blocks; by+=compptr->v_samp_factor)
				{
					JBLOCKARRAY barray=(dinfo->mem->access_virt_barray)
						((j_common_ptr)dinfo, dstcoefs[ci], by, compptr->v_samp_factor,
						TRUE);
					for(y=0; y<compptr->v_samp_factor; y++)
					{
						if(t[i].customFilter(barray[y][0], arrayRegion, planeRegion,
							ci, i, &t[i])==-1)
							_throw("tjTransform(): Error in custom filter");
						arrayRegion.y+=DCTSIZE;
					}
				}
			}
		}
		if(!(t[i].options&TJXOPT_NOOUTPUT)) jpeg_finish_compress(cinfo);
	}

	jpeg_finish_decompress(dinfo);

	bailout:
	if(cinfo->global_state>CSTATE_START) jpeg_abort_compress(cinfo);
	if(dinfo->global_state>DSTATE_START) jpeg_abort_decompress(dinfo);
	if(xinfo) free(xinfo);
	if(this->jerr.warning) retval=-1;
	this->jerr.stopOnWarning=FALSE;
	return retval;
}