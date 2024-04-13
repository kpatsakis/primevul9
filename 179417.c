static inline void compat_sig_setmask(sigset_t *blocked, compat_sigset_word set)
{
	memcpy(blocked->sig, &set, sizeof(set));
}