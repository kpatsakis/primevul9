static int getSubsamp(j_decompress_ptr dinfo)
{
	int retval=-1, i, k;

	/* The sampling factors actually have no meaning with grayscale JPEG files,
	   and in fact it's possible to generate grayscale JPEGs with sampling
	   factors > 1 (even though those sampling factors are ignored by the
	   decompressor.)  Thus, we need to treat grayscale as a special case. */
	if(dinfo->num_components==1 && dinfo->jpeg_color_space==JCS_GRAYSCALE)
		return TJSAMP_GRAY;

	for(i=0; i<NUMSUBOPT; i++)
	{
		if(dinfo->num_components==pixelsize[i]
			|| ((dinfo->jpeg_color_space==JCS_YCCK
				|| dinfo->jpeg_color_space==JCS_CMYK)
					&& pixelsize[i]==3 && dinfo->num_components==4))
		{
			if(dinfo->comp_info[0].h_samp_factor==tjMCUWidth[i]/8
				&& dinfo->comp_info[0].v_samp_factor==tjMCUHeight[i]/8)
			{
				int match=0;
				for(k=1; k<dinfo->num_components; k++)
				{
					int href=1, vref=1;
					if(dinfo->jpeg_color_space==JCS_YCCK && k==3)
					{
						href=tjMCUWidth[i]/8;  vref=tjMCUHeight[i]/8;
					}
					if(dinfo->comp_info[k].h_samp_factor==href
						&& dinfo->comp_info[k].v_samp_factor==vref)
						match++;
				}
				if(match==dinfo->num_components-1)
				{
					retval=i;  break;
				}
			}
			/* Handle 4:2:2 and 4:4:0 images whose sampling factors are specified
			   in non-standard ways. */
			if(dinfo->comp_info[0].h_samp_factor==2 &&
				dinfo->comp_info[0].v_samp_factor==2 &&
				(i==TJSAMP_422 || i==TJSAMP_440))
			{
				int match=0;
				for(k=1; k<dinfo->num_components; k++)
				{
					int href=tjMCUHeight[i]/8, vref=tjMCUWidth[i]/8;
					if(dinfo->jpeg_color_space==JCS_YCCK && k==3)
					{
						href=vref=2;
					}
					if(dinfo->comp_info[k].h_samp_factor==href
						&& dinfo->comp_info[k].v_samp_factor==vref)
						match++;
				}
				if(match==dinfo->num_components-1)
				{
					retval=i;  break;
				}
			}
		}
	}
	return retval;
}