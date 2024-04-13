static inline void __fpregs_load_activate(void)
{
	struct fpu *fpu = &current->thread.fpu;
	int cpu = smp_processor_id();

	if (WARN_ON_ONCE(current->flags & PF_KTHREAD))
		return;

	if (!fpregs_state_valid(fpu, cpu)) {
		copy_kernel_to_fpregs(&fpu->state);
		fpregs_activate(fpu);
		fpu->last_cpu = cpu;
	}
	clear_thread_flag(TIF_NEED_FPU_LOAD);
}