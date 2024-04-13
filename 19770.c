int sev_es_string_io(struct vcpu_svm *svm, int size, unsigned int port, int in)
{
	int count;
	int bytes;

	if (svm->vmcb->control.exit_info_2 > INT_MAX)
		return -EINVAL;

	count = svm->vmcb->control.exit_info_2;
	if (unlikely(check_mul_overflow(count, size, &bytes)))
		return -EINVAL;

	if (!setup_vmgexit_scratch(svm, in, bytes))
		return -EINVAL;

	return kvm_sev_es_string_io(&svm->vcpu, size, port, svm->ghcb_sa, count, in);
}