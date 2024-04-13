static u32 alloc_apf_token(struct kvm_vcpu *vcpu)
{
	/* make sure the token value is not 0 */
	u32 id = vcpu->arch.apf.id;

	if (id << 12 == 0)
		vcpu->arch.apf.id = 1;

	return (vcpu->arch.apf.id++ << 12) | vcpu->vcpu_id;
}