int nested_svm_vmrun(struct kvm_vcpu *vcpu)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	int ret;
	struct vmcb *vmcb12;
	struct kvm_host_map map;
	u64 vmcb12_gpa;

	if (!svm->nested.hsave_msr) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	}

	if (is_smm(vcpu)) {
		kvm_queue_exception(vcpu, UD_VECTOR);
		return 1;
	}

	vmcb12_gpa = svm->vmcb->save.rax;
	ret = kvm_vcpu_map(vcpu, gpa_to_gfn(vmcb12_gpa), &map);
	if (ret == -EINVAL) {
		kvm_inject_gp(vcpu, 0);
		return 1;
	} else if (ret) {
		return kvm_skip_emulated_instruction(vcpu);
	}

	ret = kvm_skip_emulated_instruction(vcpu);

	vmcb12 = map.hva;

	if (WARN_ON_ONCE(!svm->nested.initialized))
		return -EINVAL;

	nested_load_control_from_vmcb12(svm, &vmcb12->control);

	if (!nested_vmcb_valid_sregs(vcpu, &vmcb12->save) ||
	    !nested_vmcb_check_controls(vcpu, &svm->nested.ctl)) {
		vmcb12->control.exit_code    = SVM_EXIT_ERR;
		vmcb12->control.exit_code_hi = 0;
		vmcb12->control.exit_info_1  = 0;
		vmcb12->control.exit_info_2  = 0;
		goto out;
	}


	/* Clear internal status */
	kvm_clear_exception_queue(vcpu);
	kvm_clear_interrupt_queue(vcpu);

	/*
	 * Since vmcb01 is not in use, we can use it to store some of the L1
	 * state.
	 */
	svm->vmcb01.ptr->save.efer   = vcpu->arch.efer;
	svm->vmcb01.ptr->save.cr0    = kvm_read_cr0(vcpu);
	svm->vmcb01.ptr->save.cr4    = vcpu->arch.cr4;
	svm->vmcb01.ptr->save.rflags = kvm_get_rflags(vcpu);
	svm->vmcb01.ptr->save.rip    = kvm_rip_read(vcpu);

	if (!npt_enabled)
		svm->vmcb01.ptr->save.cr3 = kvm_read_cr3(vcpu);

	svm->nested.nested_run_pending = 1;

	if (enter_svm_guest_mode(vcpu, vmcb12_gpa, vmcb12))
		goto out_exit_err;

	if (nested_svm_vmrun_msrpm(svm))
		goto out;

out_exit_err:
	svm->nested.nested_run_pending = 0;

	svm->vmcb->control.exit_code    = SVM_EXIT_ERR;
	svm->vmcb->control.exit_code_hi = 0;
	svm->vmcb->control.exit_info_1  = 0;
	svm->vmcb->control.exit_info_2  = 0;

	nested_svm_vmexit(svm);

out:
	kvm_vcpu_unmap(vcpu, &map, true);

	return ret;
}