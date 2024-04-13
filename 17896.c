jas_image_t *converttosrgb(jas_image_t *inimage)
{
	jas_image_t *outimage;
	jas_cmpixmap_t inpixmap;
	jas_cmpixmap_t outpixmap;
	jas_cmcmptfmt_t incmptfmts[16];
	jas_cmcmptfmt_t outcmptfmts[16];

	outprof = jas_cmprof_createfromclrspc(JAS_CLRSPC_SRGB);
	assert(outprof);
	xform = jas_cmxform_create(jas_image_cmprof(inimage), outprof, 0, JAS_CMXFORM_FWD, JAS_CMXFORM_INTENT_PER, JAS_CMXFORM_OPTM_SPEED);
	assert(xform);

	inpixmap.numcmpts = jas_image_numcmpts(oldimage);
	outpixmap.numcmpts = 3;
	for (i = 0; i < inpixmap.numcmpts; ++i) {
		inpixmap.cmptfmts[i] = &incmptfmts[i];
	}
	for (i = 0; i < outpixmap.numcmpts; ++i)
		outpixmap.cmptfmts[i] = &outcmptfmts[i];
	if (jas_cmxform_apply(xform, &inpixmap, &outpixmap))
		abort();

	jas_xform_destroy(xform);
	jas_cmprof_destroy(outprof);
	return 0;
}