static void * __init check_one_pgsz(struct mdesc_handle *hp, u64 mp, int cpuid, void *arg)
{
	const u64 *pgsz_prop = mdesc_get_property(hp, mp, "mmu-page-size-list", NULL);
	unsigned long *pgsz_mask = arg;
	u64 val;

	val = (HV_PGSZ_MASK_8K | HV_PGSZ_MASK_64K |
	       HV_PGSZ_MASK_512K | HV_PGSZ_MASK_4MB);
	if (pgsz_prop)
		val = *pgsz_prop;

	if (!*pgsz_mask)
		*pgsz_mask = val;
	else
		*pgsz_mask &= val;
	return NULL;
}