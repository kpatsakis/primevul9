static int ras_putdata(jas_stream_t *out, ras_hdr_t *hdr, jas_image_t *image, int numcmpts, int *cmpts)
{
	int ret;

	switch (hdr->type) {
	case RAS_TYPE_STD:
		ret = ras_putdatastd(out, hdr, image, numcmpts, cmpts);
		break;
	default:
		ret = -1;
		break;
	}
	return ret;
}