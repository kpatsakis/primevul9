static void nested_vmcb02_prepare_save(struct vcpu_svm *svm, struct vmcb *vmcb12)
{
	bool new_vmcb12 = false;

	nested_vmcb02_compute_g_pat(svm);

	/* Load the nested guest state */
	if (svm->nested.vmcb12_gpa != svm->nested.last_vmcb12_gpa) {
		new_vmcb12 = true;
		svm->nested.last_vmcb12_gpa = svm->nested.vmcb12_gpa;
	}

	if (unlikely(new_vmcb12 || vmcb_is_dirty(vmcb12, VMCB_SEG))) {
		svm->vmcb->save.es = vmcb12->save.es;
		svm->vmcb->save.cs = vmcb12->save.cs;
		svm->vmcb->save.ss = vmcb12->save.ss;
		svm->vmcb->save.ds = vmcb12->save.ds;
		svm->vmcb->save.cpl = vmcb12->save.cpl;
		vmcb_mark_dirty(svm->vmcb, VMCB_SEG);
	}

	if (unlikely(new_vmcb12 || vmcb_is_dirty(vmcb12, VMCB_DT))) {
		svm->vmcb->save.gdtr = vmcb12->save.gdtr;
		svm->vmcb->save.idtr = vmcb12->save.idtr;
		vmcb_mark_dirty(svm->vmcb, VMCB_DT);
	}

	kvm_set_rflags(&svm->vcpu, vmcb12->save.rflags | X86_EFLAGS_FIXED);

	/*
	 * Force-set EFER_SVME even though it is checked earlier on the
	 * VMCB12, because the guest can flip the bit between the check
	 * and now.  Clearing EFER_SVME would call svm_free_nested.
	 */
	svm_set_efer(&svm->vcpu, vmcb12->save.efer | EFER_SVME);

	svm_set_cr0(&svm->vcpu, vmcb12->save.cr0);
	svm_set_cr4(&svm->vcpu, vmcb12->save.cr4);

	svm->vcpu.arch.cr2 = vmcb12->save.cr2;

	kvm_rax_write(&svm->vcpu, vmcb12->save.rax);
	kvm_rsp_write(&svm->vcpu, vmcb12->save.rsp);
	kvm_rip_write(&svm->vcpu, vmcb12->save.rip);

	/* In case we don't even reach vcpu_run, the fields are not updated */
	svm->vmcb->save.rax = vmcb12->save.rax;
	svm->vmcb->save.rsp = vmcb12->save.rsp;
	svm->vmcb->save.rip = vmcb12->save.rip;

	/* These bits will be set properly on the first execution when new_vmc12 is true */
	if (unlikely(new_vmcb12 || vmcb_is_dirty(vmcb12, VMCB_DR))) {
		svm->vmcb->save.dr7 = vmcb12->save.dr7 | DR7_FIXED_1;
		svm->vcpu.arch.dr6  = vmcb12->save.dr6 | DR6_ACTIVE_LOW;
		vmcb_mark_dirty(svm->vmcb, VMCB_DR);
	}
}