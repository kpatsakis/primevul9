int jas_image_encode(jas_image_t *image, jas_stream_t *out, int fmt,
  const char *optstr)
{
	jas_image_fmtinfo_t *fmtinfo;
	if (!(fmtinfo = jas_image_lookupfmtbyid(fmt))) {
		jas_eprintf("format lookup failed\n");
		return -1;
	}
	return (fmtinfo->ops.encode) ? (*fmtinfo->ops.encode)(image, out,
	  optstr) : (-1);
}