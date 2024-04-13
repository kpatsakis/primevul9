int svm_unregister_enc_region(struct kvm *kvm,
			      struct kvm_enc_region *range)
{
	struct enc_region *region;
	int ret;

	/* If kvm is mirroring encryption context it isn't responsible for it */
	if (is_mirroring_enc_context(kvm))
		return -EINVAL;

	mutex_lock(&kvm->lock);

	if (!sev_guest(kvm)) {
		ret = -ENOTTY;
		goto failed;
	}

	region = find_enc_region(kvm, range);
	if (!region) {
		ret = -EINVAL;
		goto failed;
	}

	/*
	 * Ensure that all guest tagged cache entries are flushed before
	 * releasing the pages back to the system for use. CLFLUSH will
	 * not do this, so issue a WBINVD.
	 */
	wbinvd_on_all_cpus();

	__unregister_enc_region_locked(kvm, region);

	mutex_unlock(&kvm->lock);
	return 0;

failed:
	mutex_unlock(&kvm->lock);
	return ret;
}