void __init sev_hardware_setup(void)
{
#ifdef CONFIG_KVM_AMD_SEV
	unsigned int eax, ebx, ecx, edx, sev_asid_count, sev_es_asid_count;
	bool sev_es_supported = false;
	bool sev_supported = false;

	if (!sev_enabled || !npt_enabled)
		goto out;

	/* Does the CPU support SEV? */
	if (!boot_cpu_has(X86_FEATURE_SEV))
		goto out;

	/* Retrieve SEV CPUID information */
	cpuid(0x8000001f, &eax, &ebx, &ecx, &edx);

	/* Set encryption bit location for SEV-ES guests */
	sev_enc_bit = ebx & 0x3f;

	/* Maximum number of encrypted guests supported simultaneously */
	max_sev_asid = ecx;
	if (!max_sev_asid)
		goto out;

	/* Minimum ASID value that should be used for SEV guest */
	min_sev_asid = edx;
	sev_me_mask = 1UL << (ebx & 0x3f);

	/*
	 * Initialize SEV ASID bitmaps. Allocate space for ASID 0 in the bitmap,
	 * even though it's never used, so that the bitmap is indexed by the
	 * actual ASID.
	 */
	nr_asids = max_sev_asid + 1;
	sev_asid_bitmap = bitmap_zalloc(nr_asids, GFP_KERNEL);
	if (!sev_asid_bitmap)
		goto out;

	sev_reclaim_asid_bitmap = bitmap_zalloc(nr_asids, GFP_KERNEL);
	if (!sev_reclaim_asid_bitmap) {
		bitmap_free(sev_asid_bitmap);
		sev_asid_bitmap = NULL;
		goto out;
	}

	sev_asid_count = max_sev_asid - min_sev_asid + 1;
	if (misc_cg_set_capacity(MISC_CG_RES_SEV, sev_asid_count))
		goto out;

	pr_info("SEV supported: %u ASIDs\n", sev_asid_count);
	sev_supported = true;

	/* SEV-ES support requested? */
	if (!sev_es_enabled)
		goto out;

	/* Does the CPU support SEV-ES? */
	if (!boot_cpu_has(X86_FEATURE_SEV_ES))
		goto out;

	/* Has the system been allocated ASIDs for SEV-ES? */
	if (min_sev_asid == 1)
		goto out;

	sev_es_asid_count = min_sev_asid - 1;
	if (misc_cg_set_capacity(MISC_CG_RES_SEV_ES, sev_es_asid_count))
		goto out;

	pr_info("SEV-ES supported: %u ASIDs\n", sev_es_asid_count);
	sev_es_supported = true;

out:
	sev_enabled = sev_supported;
	sev_es_enabled = sev_es_supported;
#endif
}