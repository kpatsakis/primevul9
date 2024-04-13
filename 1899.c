static void nested_svm_triple_fault(struct kvm_vcpu *vcpu)
{
	nested_svm_simple_vmexit(to_svm(vcpu), SVM_EXIT_SHUTDOWN);
}