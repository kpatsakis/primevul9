static bool kvmgt_is_valid_gfn(unsigned long handle, unsigned long gfn)
{
	struct kvmgt_guest_info *info;
	struct kvm *kvm;
	int idx;
	bool ret;

	if (!handle_valid(handle))
		return false;

	info = (struct kvmgt_guest_info *)handle;
	kvm = info->kvm;

	idx = srcu_read_lock(&kvm->srcu);
	ret = kvm_is_visible_gfn(kvm, gfn);
	srcu_read_unlock(&kvm->srcu, idx);

	return ret;
}