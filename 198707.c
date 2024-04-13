static unsigned int tw5864_md_metric_from_mvd(u32 mvd)
{
	/*
	 * Format of motion vector data exposed by tw5864, according to
	 * manufacturer:
	 * mv_x 10 bits
	 * mv_y 10 bits
	 * non_zero_members 8 bits
	 * mb_type 3 bits
	 * reserved 1 bit
	 *
	 * non_zero_members: number of non-zero residuals in each macro block
	 * after quantization
	 *
	 * unsigned int reserved = mvd >> 31;
	 * unsigned int mb_type = (mvd >> 28) & 0x7;
	 * unsigned int non_zero_members = (mvd >> 20) & 0xff;
	 */
	unsigned int mv_y = (mvd >> 10) & 0x3ff;
	unsigned int mv_x = mvd & 0x3ff;

	/* heuristic: */
	mv_x &= 0x0f;
	mv_y &= 0x0f;

	return mv_y + mv_x;
}