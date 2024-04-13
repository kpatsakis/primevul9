static unsigned long get_cr3(struct kvm_vcpu *vcpu)
{
	return kvm_read_cr3(vcpu);
}