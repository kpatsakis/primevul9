void sev_hardware_teardown(void)
{
	if (!sev_enabled)
		return;

	/* No need to take sev_bitmap_lock, all VMs have been destroyed. */
	sev_flush_asids(1, max_sev_asid);

	bitmap_free(sev_asid_bitmap);
	bitmap_free(sev_reclaim_asid_bitmap);

	misc_cg_set_capacity(MISC_CG_RES_SEV, 0);
	misc_cg_set_capacity(MISC_CG_RES_SEV_ES, 0);
}