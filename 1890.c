static void nested_svm_copy_common_state(struct vmcb *from_vmcb, struct vmcb *to_vmcb)
{
	/*
	 * Some VMCB state is shared between L1 and L2 and thus has to be
	 * moved at the time of nested vmrun and vmexit.
	 *
	 * VMLOAD/VMSAVE state would also belong in this category, but KVM
	 * always performs VMLOAD and VMSAVE from the VMCB01.
	 */
	to_vmcb->save.spec_ctrl = from_vmcb->save.spec_ctrl;
}