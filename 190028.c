static gfn_t pse36_gfn_delta(u32 gpte)
{
	int shift = 32 - PT32_DIR_PSE36_SHIFT - PAGE_SHIFT;

	return (gpte & PT32_DIR_PSE36_MASK) << shift;
}