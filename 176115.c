adv_error adv_png_write_iend(adv_fz* f, unsigned* count)
{
	if (adv_png_write_chunk(f, ADV_PNG_CN_IEND, 0, 0, count)!=0)
		return -1;

	return 0;
}