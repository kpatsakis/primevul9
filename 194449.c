static inline void fpstate_init_fxstate(struct fxregs_state *fx)
{
	fx->cwd = 0x37f;
	fx->mxcsr = MXCSR_DEFAULT;
}