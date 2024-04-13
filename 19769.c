int svm_vm_copy_asid_from(struct kvm *kvm, unsigned int source_fd)
{
	struct file *source_kvm_file;
	struct kvm *source_kvm;
	struct kvm_sev_info source_sev, *mirror_sev;
	int ret;

	source_kvm_file = fget(source_fd);
	if (!file_is_kvm(source_kvm_file)) {
		ret = -EBADF;
		goto e_source_put;
	}

	source_kvm = source_kvm_file->private_data;
	mutex_lock(&source_kvm->lock);

	if (!sev_guest(source_kvm)) {
		ret = -EINVAL;
		goto e_source_unlock;
	}

	/* Mirrors of mirrors should work, but let's not get silly */
	if (is_mirroring_enc_context(source_kvm) || source_kvm == kvm) {
		ret = -EINVAL;
		goto e_source_unlock;
	}

	memcpy(&source_sev, &to_kvm_svm(source_kvm)->sev_info,
	       sizeof(source_sev));

	/*
	 * The mirror kvm holds an enc_context_owner ref so its asid can't
	 * disappear until we're done with it
	 */
	kvm_get_kvm(source_kvm);

	fput(source_kvm_file);
	mutex_unlock(&source_kvm->lock);
	mutex_lock(&kvm->lock);

	if (sev_guest(kvm)) {
		ret = -EINVAL;
		goto e_mirror_unlock;
	}

	/* Set enc_context_owner and copy its encryption context over */
	mirror_sev = &to_kvm_svm(kvm)->sev_info;
	mirror_sev->enc_context_owner = source_kvm;
	mirror_sev->active = true;
	mirror_sev->asid = source_sev.asid;
	mirror_sev->fd = source_sev.fd;
	mirror_sev->es_active = source_sev.es_active;
	mirror_sev->handle = source_sev.handle;
	/*
	 * Do not copy ap_jump_table. Since the mirror does not share the same
	 * KVM contexts as the original, and they may have different
	 * memory-views.
	 */

	mutex_unlock(&kvm->lock);
	return 0;

e_mirror_unlock:
	mutex_unlock(&kvm->lock);
	kvm_put_kvm(source_kvm);
	return ret;
e_source_unlock:
	mutex_unlock(&source_kvm->lock);
e_source_put:
	if (source_kvm_file)
		fput(source_kvm_file);
	return ret;
}