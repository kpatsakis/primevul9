static int __init svm_init(void)
{
	return kvm_init(&svm_init_ops, sizeof(struct vcpu_svm),
			__alignof__(struct vcpu_svm), THIS_MODULE);
}