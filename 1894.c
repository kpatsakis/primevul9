static inline bool nested_exit_on_init(struct vcpu_svm *svm)
{
	return vmcb_is_intercept(&svm->nested.ctl, INTERCEPT_INIT);
}