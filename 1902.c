static bool nested_exit_on_exception(struct vcpu_svm *svm)
{
	unsigned int nr = svm->vcpu.arch.exception.nr;

	return (svm->nested.ctl.intercepts[INTERCEPT_EXCEPTION] & BIT(nr));
}