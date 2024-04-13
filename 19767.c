static int sev_launch_update_vmsa(struct kvm *kvm, struct kvm_sev_cmd *argp)
{
	struct kvm_vcpu *vcpu;
	int i, ret;

	if (!sev_es_guest(kvm))
		return -ENOTTY;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		ret = mutex_lock_killable(&vcpu->mutex);
		if (ret)
			return ret;

		ret = __sev_launch_update_vmsa(kvm, vcpu, &argp->error);

		mutex_unlock(&vcpu->mutex);
		if (ret)
			return ret;
	}

	return 0;
}