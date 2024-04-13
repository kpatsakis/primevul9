static unsigned get_mmio_spte_access(u64 spte)
{
	return spte & shadow_mmio_access_mask;
}