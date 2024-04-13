static inline void clear_user_asce(void)
{
	S390_lowcore.user_asce = S390_lowcore.kernel_asce;

	__ctl_load(S390_lowcore.user_asce, 1, 1);
	__ctl_load(S390_lowcore.user_asce, 7, 7);
}