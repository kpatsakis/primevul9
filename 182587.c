static uint_fast32_t jp2_gettypeasoc(int colorspace, int ctype)
{
	int type;
	int asoc;

	if (ctype & JAS_IMAGE_CT_OPACITY) {
		type = JP2_CDEF_TYPE_OPACITY;
		asoc = JP2_CDEF_ASOC_ALL;
		goto done;
	}

	type = JP2_CDEF_TYPE_UNSPEC;
	asoc = JP2_CDEF_ASOC_NONE;
	switch (jas_clrspc_fam(colorspace)) {
	case JAS_CLRSPC_FAM_RGB:
		switch (JAS_IMAGE_CT_COLOR(ctype)) {
		case JAS_IMAGE_CT_RGB_R:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_RGB_R;
			break;
		case JAS_IMAGE_CT_RGB_G:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_RGB_G;
			break;
		case JAS_IMAGE_CT_RGB_B:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_RGB_B;
			break;
		}
		break;
	case JAS_CLRSPC_FAM_YCBCR:
		switch (JAS_IMAGE_CT_COLOR(ctype)) {
		case JAS_IMAGE_CT_YCBCR_Y:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_YCBCR_Y;
			break;
		case JAS_IMAGE_CT_YCBCR_CB:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_YCBCR_CB;
			break;
		case JAS_IMAGE_CT_YCBCR_CR:
			type = JP2_CDEF_TYPE_COLOR;
			asoc = JP2_CDEF_YCBCR_CR;
			break;
		}
		break;
	case JAS_CLRSPC_FAM_GRAY:
		type = JP2_CDEF_TYPE_COLOR;
		asoc = JP2_CDEF_GRAY_Y;
		break;
	}

done:
	return (type << 16) | asoc;
}