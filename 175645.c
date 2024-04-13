static int kvmgt_rw_gpa(unsigned long handle, unsigned long gpa,
			void *buf, unsigned long len, bool write)
{
	struct kvmgt_guest_info *info;
	struct kvm *kvm;
	int idx, ret;
	bool kthread = current->mm == NULL;

	if (!handle_valid(handle))
		return -ESRCH;

	info = (struct kvmgt_guest_info *)handle;
	kvm = info->kvm;

	if (kthread) {
		if (!mmget_not_zero(kvm->mm))
			return -EFAULT;
		use_mm(kvm->mm);
	}

	idx = srcu_read_lock(&kvm->srcu);
	ret = write ? kvm_write_guest(kvm, gpa, buf, len) :
		      kvm_read_guest(kvm, gpa, buf, len);
	srcu_read_unlock(&kvm->srcu, idx);

	if (kthread) {
		unuse_mm(kvm->mm);
		mmput(kvm->mm);
	}

	return ret;
}