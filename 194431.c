static __always_inline __pure bool use_fxsr(void)
{
	return static_cpu_has(X86_FEATURE_FXSR);
}