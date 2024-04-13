get_compat_sigset(sigset_t *set, const compat_sigset_t __user *compat)
{
#ifdef __BIG_ENDIAN
	compat_sigset_t v;
	if (copy_from_user(&v, compat, sizeof(compat_sigset_t)))
		return -EFAULT;
	switch (_NSIG_WORDS) {
	case 4: set->sig[3] = v.sig[6] | (((long)v.sig[7]) << 32 );
	case 3: set->sig[2] = v.sig[4] | (((long)v.sig[5]) << 32 );
	case 2: set->sig[1] = v.sig[2] | (((long)v.sig[3]) << 32 );
	case 1: set->sig[0] = v.sig[0] | (((long)v.sig[1]) << 32 );
	}
#else
	if (copy_from_user(set, compat, sizeof(compat_sigset_t)))
		return -EFAULT;
#endif
	return 0;
}