static void svm_set_vintr(struct vcpu_svm *svm)
{
	set_intercept(svm, INTERCEPT_VINTR);
	if (is_intercept(svm, INTERCEPT_VINTR))
		svm_enable_vintr(svm);
}