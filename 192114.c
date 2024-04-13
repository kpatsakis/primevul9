static bool mce_is_correctable(struct mce *m)
{
	if (m->cpuvendor == X86_VENDOR_AMD && m->status & MCI_STATUS_DEFERRED)
		return false;

	if (m->status & MCI_STATUS_UC)
		return false;

	return true;
}