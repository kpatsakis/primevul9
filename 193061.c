sigset_from_compat(sigset_t *set, const compat_sigset_t *compat)
{
	switch (_NSIG_WORDS) {
	case 4: set->sig[3] = compat->sig[6] | (((long)compat->sig[7]) << 32 );
	case 3: set->sig[2] = compat->sig[4] | (((long)compat->sig[5]) << 32 );
	case 2: set->sig[1] = compat->sig[2] | (((long)compat->sig[3]) << 32 );
	case 1: set->sig[0] = compat->sig[0] | (((long)compat->sig[1]) << 32 );
	}
}