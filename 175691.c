int ras_encode(jas_image_t *image, jas_stream_t *out, const char *optstr)
{
	int_fast32_t width;
	int_fast32_t height;
	int_fast32_t depth;
	int cmptno;
	int i;
	ras_hdr_t hdr;
	int rowsize;
	ras_enc_t encbuf;
	ras_enc_t *enc = &encbuf;

	JAS_DBGLOG(10, ("ras_encode(%p, %p, \"%s\"\n", image, out,
	  optstr ? optstr : ""));

	if (optstr) {
		jas_eprintf("warning: ignoring RAS encoder options\n");
	}

	switch (jas_clrspc_fam(jas_image_clrspc(image))) {
	case JAS_CLRSPC_FAM_RGB:
		if (jas_image_clrspc(image) != JAS_CLRSPC_SRGB)
			jas_eprintf("warning: inaccurate color\n");
		enc->numcmpts = 3;
		if ((enc->cmpts[0] = jas_image_getcmptbytype(image,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R))) < 0 ||
		  (enc->cmpts[1] = jas_image_getcmptbytype(image,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G))) < 0 ||
		  (enc->cmpts[2] = jas_image_getcmptbytype(image,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B))) < 0) {
			jas_eprintf("error: missing color component\n");
			return -1;
		}
		break;
	case JAS_CLRSPC_FAM_GRAY:
		if (jas_image_clrspc(image) != JAS_CLRSPC_SGRAY)
			jas_eprintf("warning: inaccurate color\n");
		enc->numcmpts = 1;
		if ((enc->cmpts[0] = jas_image_getcmptbytype(image,
		  JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_GRAY_Y))) < 0) {
			jas_eprintf("error: missing color component\n");
			return -1;
		}
		break;
	default:
		jas_eprintf("error: unsupported color space\n");
		return -1;
		break;
	}

	width = jas_image_cmptwidth(image, enc->cmpts[0]);
	height = jas_image_cmptheight(image, enc->cmpts[0]);
	depth = jas_image_cmptprec(image, enc->cmpts[0]);

	for (cmptno = 0; cmptno < enc->numcmpts; ++cmptno) {
		if (jas_image_cmptwidth(image, enc->cmpts[cmptno]) != width ||
		  jas_image_cmptheight(image, enc->cmpts[cmptno]) != height ||
		  jas_image_cmptprec(image, enc->cmpts[cmptno]) != depth ||
		  jas_image_cmptsgnd(image, enc->cmpts[cmptno]) != false ||
		  jas_image_cmpttlx(image, enc->cmpts[cmptno]) != 0 ||
		  jas_image_cmpttly(image, enc->cmpts[cmptno]) != 0) {
			jas_eprintf("The RAS format cannot be used to represent an image with this geometry.\n");
			return -1;
		}
	}

	/* Ensure that the image can be encoded in the desired format. */
	if (enc->numcmpts == 3) {

		/* All three components must have the same subsampling
		  factor and have a precision of eight bits. */
		if (enc->numcmpts > 1) {
			for (i = 0; i < enc->numcmpts; ++i) {
				if (jas_image_cmptprec(image, enc->cmpts[i]) != 8) {
					return -1;
				}
			}
		}
	} else if (enc->numcmpts == 1) {
		/* NOP */
	} else {
		return -1;
	}

	hdr.magic = RAS_MAGIC;
	hdr.width = width;
	hdr.height = height;
	hdr.depth = (enc->numcmpts == 3) ? 24 : depth;

	rowsize = RAS_ROWSIZE(&hdr);
	hdr.length = rowsize * hdr.height;
	hdr.type = RAS_TYPE_STD;

	hdr.maptype = RAS_MT_NONE;
	hdr.maplength = 0;

	if (ras_puthdr(out, &hdr)) {
		return -1;
	}

	if (ras_putdata(out, &hdr, image, enc->numcmpts, enc->cmpts)) {
		return -1;
	}

	return 0;
}