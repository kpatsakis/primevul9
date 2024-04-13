static void toggle_interruptibility(struct x86_emulate_ctxt *ctxt, u32 mask)
{
	u32 int_shadow = kvm_x86_ops->get_interrupt_shadow(ctxt->vcpu, mask);
	/*
	 * an sti; sti; sequence only disable interrupts for the first
	 * instruction. So, if the last instruction, be it emulated or
	 * not, left the system with the INT_STI flag enabled, it
	 * means that the last instruction is an sti. We should not
	 * leave the flag on in this case. The same goes for mov ss
	 */
	if (!(int_shadow & mask))
		ctxt->interruptibility = mask;
}