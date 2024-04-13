static int mce_usable_address(struct mce *m)
{
	if (!(m->status & MCI_STATUS_ADDRV))
		return 0;

	/* Checks after this one are Intel-specific: */
	if (boot_cpu_data.x86_vendor != X86_VENDOR_INTEL)
		return 1;

	if (!(m->status & MCI_STATUS_MISCV))
		return 0;

	if (MCI_MISC_ADDR_LSB(m->misc) > PAGE_SHIFT)
		return 0;

	if (MCI_MISC_ADDR_MODE(m->misc) != MCI_MISC_ADDR_PHYS)
		return 0;

	return 1;
}