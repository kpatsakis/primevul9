static int svm_set_nested_state(struct kvm_vcpu *vcpu,
				struct kvm_nested_state __user *user_kvm_nested_state,
				struct kvm_nested_state *kvm_state)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	struct vmcb __user *user_vmcb = (struct vmcb __user *)
		&user_kvm_nested_state->data.svm[0];
	struct vmcb_control_area *ctl;
	struct vmcb_save_area *save;
	unsigned long cr0;
	int ret;

	BUILD_BUG_ON(sizeof(struct vmcb_control_area) + sizeof(struct vmcb_save_area) >
		     KVM_STATE_NESTED_SVM_VMCB_SIZE);

	if (kvm_state->format != KVM_STATE_NESTED_FORMAT_SVM)
		return -EINVAL;

	if (kvm_state->flags & ~(KVM_STATE_NESTED_GUEST_MODE |
				 KVM_STATE_NESTED_RUN_PENDING |
				 KVM_STATE_NESTED_GIF_SET))
		return -EINVAL;

	/*
	 * If in guest mode, vcpu->arch.efer actually refers to the L2 guest's
	 * EFER.SVME, but EFER.SVME still has to be 1 for VMRUN to succeed.
	 */
	if (!(vcpu->arch.efer & EFER_SVME)) {
		/* GIF=1 and no guest mode are required if SVME=0.  */
		if (kvm_state->flags != KVM_STATE_NESTED_GIF_SET)
			return -EINVAL;
	}

	/* SMM temporarily disables SVM, so we cannot be in guest mode.  */
	if (is_smm(vcpu) && (kvm_state->flags & KVM_STATE_NESTED_GUEST_MODE))
		return -EINVAL;

	if (!(kvm_state->flags & KVM_STATE_NESTED_GUEST_MODE)) {
		svm_leave_nested(svm);
		svm_set_gif(svm, !!(kvm_state->flags & KVM_STATE_NESTED_GIF_SET));
		return 0;
	}

	if (!page_address_valid(vcpu, kvm_state->hdr.svm.vmcb_pa))
		return -EINVAL;
	if (kvm_state->size < sizeof(*kvm_state) + KVM_STATE_NESTED_SVM_VMCB_SIZE)
		return -EINVAL;

	ret  = -ENOMEM;
	ctl  = kzalloc(sizeof(*ctl),  GFP_KERNEL_ACCOUNT);
	save = kzalloc(sizeof(*save), GFP_KERNEL_ACCOUNT);
	if (!ctl || !save)
		goto out_free;

	ret = -EFAULT;
	if (copy_from_user(ctl, &user_vmcb->control, sizeof(*ctl)))
		goto out_free;
	if (copy_from_user(save, &user_vmcb->save, sizeof(*save)))
		goto out_free;

	ret = -EINVAL;
	if (!nested_vmcb_check_controls(vcpu, ctl))
		goto out_free;

	/*
	 * Processor state contains L2 state.  Check that it is
	 * valid for guest mode (see nested_vmcb_check_save).
	 */
	cr0 = kvm_read_cr0(vcpu);
        if (((cr0 & X86_CR0_CD) == 0) && (cr0 & X86_CR0_NW))
		goto out_free;

	/*
	 * Validate host state saved from before VMRUN (see
	 * nested_svm_check_permissions).
	 */
	if (!(save->cr0 & X86_CR0_PG) ||
	    !(save->cr0 & X86_CR0_PE) ||
	    (save->rflags & X86_EFLAGS_VM) ||
	    !nested_vmcb_valid_sregs(vcpu, save))
		goto out_free;

	/*
	 * While the nested guest CR3 is already checked and set by
	 * KVM_SET_SREGS, it was set when nested state was yet loaded,
	 * thus MMU might not be initialized correctly.
	 * Set it again to fix this.
	 */

	ret = nested_svm_load_cr3(&svm->vcpu, vcpu->arch.cr3,
				  nested_npt_enabled(svm), false);
	if (WARN_ON_ONCE(ret))
		goto out_free;


	/*
	 * All checks done, we can enter guest mode. Userspace provides
	 * vmcb12.control, which will be combined with L1 and stored into
	 * vmcb02, and the L1 save state which we store in vmcb01.
	 * L2 registers if needed are moved from the current VMCB to VMCB02.
	 */

	if (is_guest_mode(vcpu))
		svm_leave_nested(svm);
	else
		svm->nested.vmcb02.ptr->save = svm->vmcb01.ptr->save;

	svm_set_gif(svm, !!(kvm_state->flags & KVM_STATE_NESTED_GIF_SET));

	svm->nested.nested_run_pending =
		!!(kvm_state->flags & KVM_STATE_NESTED_RUN_PENDING);

	svm->nested.vmcb12_gpa = kvm_state->hdr.svm.vmcb_pa;

	svm_copy_vmrun_state(&svm->vmcb01.ptr->save, save);
	nested_load_control_from_vmcb12(svm, ctl);

	svm_switch_vmcb(svm, &svm->nested.vmcb02);
	nested_vmcb02_prepare_control(svm);
	kvm_make_request(KVM_REQ_GET_NESTED_STATE_PAGES, vcpu);
	ret = 0;
out_free:
	kfree(save);
	kfree(ctl);

	return ret;
}