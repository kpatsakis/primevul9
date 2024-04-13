void svm_copy_vmrun_state(struct vmcb_save_area *to_save,
			  struct vmcb_save_area *from_save)
{
	to_save->es = from_save->es;
	to_save->cs = from_save->cs;
	to_save->ss = from_save->ss;
	to_save->ds = from_save->ds;
	to_save->gdtr = from_save->gdtr;
	to_save->idtr = from_save->idtr;
	to_save->rflags = from_save->rflags | X86_EFLAGS_FIXED;
	to_save->efer = from_save->efer;
	to_save->cr0 = from_save->cr0;
	to_save->cr3 = from_save->cr3;
	to_save->cr4 = from_save->cr4;
	to_save->rax = from_save->rax;
	to_save->rsp = from_save->rsp;
	to_save->rip = from_save->rip;
	to_save->cpl = 0;
}