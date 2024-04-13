static int kvm_vm_ioctl_enable_cap_generic(struct kvm *kvm,
					   struct kvm_enable_cap *cap)
{
	switch (cap->cap) {
#ifdef CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT
	case KVM_CAP_MANUAL_DIRTY_LOG_PROTECT2: {
		u64 allowed_options = KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE;

		if (cap->args[0] & KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE)
			allowed_options = KVM_DIRTY_LOG_MANUAL_CAPS;

		if (cap->flags || (cap->args[0] & ~allowed_options))
			return -EINVAL;
		kvm->manual_dirty_log_protect = cap->args[0];
		return 0;
	}
#endif
	case KVM_CAP_HALT_POLL: {
		if (cap->flags || cap->args[0] != (unsigned int)cap->args[0])
			return -EINVAL;

		kvm->max_halt_poll_ns = cap->args[0];
		return 0;
	}
	default:
		return kvm_vm_ioctl_enable_cap(kvm, cap);
	}
}