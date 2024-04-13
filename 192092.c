static int srao_decode_notifier(struct notifier_block *nb, unsigned long val,
				void *data)
{
	struct mce *mce = (struct mce *)data;
	unsigned long pfn;

	if (!mce)
		return NOTIFY_DONE;

	if (mce_usable_address(mce) && (mce->severity == MCE_AO_SEVERITY)) {
		pfn = mce->addr >> PAGE_SHIFT;
		if (!memory_failure(pfn, 0))
			mce_unmap_kpfn(pfn);
	}

	return NOTIFY_OK;
}