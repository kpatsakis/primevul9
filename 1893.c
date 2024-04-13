int nested_svm_vmexit(struct vcpu_svm *svm)
{
	struct kvm_vcpu *vcpu = &svm->vcpu;
	struct vmcb *vmcb12;
	struct vmcb *vmcb = svm->vmcb;
	struct kvm_host_map map;
	int rc;

	/* Triple faults in L2 should never escape. */
	WARN_ON_ONCE(kvm_check_request(KVM_REQ_TRIPLE_FAULT, vcpu));

	rc = kvm_vcpu_map(vcpu, gpa_to_gfn(svm->nested.vmcb12_gpa), &map);
	if (rc) {
		if (rc == -EINVAL)
			kvm_inject_gp(vcpu, 0);
		return 1;
	}

	vmcb12 = map.hva;

	/* Exit Guest-Mode */
	leave_guest_mode(vcpu);
	svm->nested.vmcb12_gpa = 0;
	WARN_ON_ONCE(svm->nested.nested_run_pending);

	kvm_clear_request(KVM_REQ_GET_NESTED_STATE_PAGES, vcpu);

	/* in case we halted in L2 */
	svm->vcpu.arch.mp_state = KVM_MP_STATE_RUNNABLE;

	/* Give the current vmcb to the guest */

	vmcb12->save.es     = vmcb->save.es;
	vmcb12->save.cs     = vmcb->save.cs;
	vmcb12->save.ss     = vmcb->save.ss;
	vmcb12->save.ds     = vmcb->save.ds;
	vmcb12->save.gdtr   = vmcb->save.gdtr;
	vmcb12->save.idtr   = vmcb->save.idtr;
	vmcb12->save.efer   = svm->vcpu.arch.efer;
	vmcb12->save.cr0    = kvm_read_cr0(vcpu);
	vmcb12->save.cr3    = kvm_read_cr3(vcpu);
	vmcb12->save.cr2    = vmcb->save.cr2;
	vmcb12->save.cr4    = svm->vcpu.arch.cr4;
	vmcb12->save.rflags = kvm_get_rflags(vcpu);
	vmcb12->save.rip    = kvm_rip_read(vcpu);
	vmcb12->save.rsp    = kvm_rsp_read(vcpu);
	vmcb12->save.rax    = kvm_rax_read(vcpu);
	vmcb12->save.dr7    = vmcb->save.dr7;
	vmcb12->save.dr6    = svm->vcpu.arch.dr6;
	vmcb12->save.cpl    = vmcb->save.cpl;

	vmcb12->control.int_state         = vmcb->control.int_state;
	vmcb12->control.exit_code         = vmcb->control.exit_code;
	vmcb12->control.exit_code_hi      = vmcb->control.exit_code_hi;
	vmcb12->control.exit_info_1       = vmcb->control.exit_info_1;
	vmcb12->control.exit_info_2       = vmcb->control.exit_info_2;

	if (vmcb12->control.exit_code != SVM_EXIT_ERR)
		nested_save_pending_event_to_vmcb12(svm, vmcb12);

	if (svm->nrips_enabled)
		vmcb12->control.next_rip  = vmcb->control.next_rip;

	vmcb12->control.int_ctl           = svm->nested.ctl.int_ctl;
	vmcb12->control.tlb_ctl           = svm->nested.ctl.tlb_ctl;
	vmcb12->control.event_inj         = svm->nested.ctl.event_inj;
	vmcb12->control.event_inj_err     = svm->nested.ctl.event_inj_err;

	vmcb12->control.pause_filter_count =
		svm->vmcb->control.pause_filter_count;
	vmcb12->control.pause_filter_thresh =
		svm->vmcb->control.pause_filter_thresh;

	nested_svm_copy_common_state(svm->nested.vmcb02.ptr, svm->vmcb01.ptr);

	svm_switch_vmcb(svm, &svm->vmcb01);

	/*
	 * On vmexit the  GIF is set to false and
	 * no event can be injected in L1.
	 */
	svm_set_gif(svm, false);
	svm->vmcb->control.exit_int_info = 0;

	svm->vcpu.arch.tsc_offset = svm->vcpu.arch.l1_tsc_offset;
	if (svm->vmcb->control.tsc_offset != svm->vcpu.arch.tsc_offset) {
		svm->vmcb->control.tsc_offset = svm->vcpu.arch.tsc_offset;
		vmcb_mark_dirty(svm->vmcb, VMCB_INTERCEPTS);
	}

	svm->nested.ctl.nested_cr3 = 0;

	/*
	 * Restore processor state that had been saved in vmcb01
	 */
	kvm_set_rflags(vcpu, svm->vmcb->save.rflags);
	svm_set_efer(vcpu, svm->vmcb->save.efer);
	svm_set_cr0(vcpu, svm->vmcb->save.cr0 | X86_CR0_PE);
	svm_set_cr4(vcpu, svm->vmcb->save.cr4);
	kvm_rax_write(vcpu, svm->vmcb->save.rax);
	kvm_rsp_write(vcpu, svm->vmcb->save.rsp);
	kvm_rip_write(vcpu, svm->vmcb->save.rip);

	svm->vcpu.arch.dr7 = DR7_FIXED_1;
	kvm_update_dr7(&svm->vcpu);

	trace_kvm_nested_vmexit_inject(vmcb12->control.exit_code,
				       vmcb12->control.exit_info_1,
				       vmcb12->control.exit_info_2,
				       vmcb12->control.exit_int_info,
				       vmcb12->control.exit_int_info_err,
				       KVM_ISA_SVM);

	kvm_vcpu_unmap(vcpu, &map, true);

	nested_svm_transition_tlb_flush(vcpu);

	nested_svm_uninit_mmu_context(vcpu);

	rc = nested_svm_load_cr3(vcpu, svm->vmcb->save.cr3, false, true);
	if (rc)
		return 1;

	/*
	 * Drop what we picked up for L2 via svm_complete_interrupts() so it
	 * doesn't end up in L1.
	 */
	svm->vcpu.arch.nmi_injected = false;
	kvm_clear_exception_queue(vcpu);
	kvm_clear_interrupt_queue(vcpu);

	/*
	 * If we are here following the completion of a VMRUN that
	 * is being single-stepped, queue the pending #DB intercept
	 * right now so that it an be accounted for before we execute
	 * L1's next instruction.
	 */
	if (unlikely(svm->vmcb->save.rflags & X86_EFLAGS_TF))
		kvm_queue_exception(&(svm->vcpu), DB_VECTOR);

	return 0;
}