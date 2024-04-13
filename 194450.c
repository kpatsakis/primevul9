static inline void switch_fpu_prepare(struct fpu *old_fpu, int cpu)
{
	if (static_cpu_has(X86_FEATURE_FPU) && !(current->flags & PF_KTHREAD)) {
		if (!copy_fpregs_to_fpstate(old_fpu))
			old_fpu->last_cpu = -1;
		else
			old_fpu->last_cpu = cpu;

		/* But leave fpu_fpregs_owner_ctx! */
		trace_x86_fpu_regs_deactivated(old_fpu);
	}
}