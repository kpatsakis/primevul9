static int svm_check_intercept(struct kvm_vcpu *vcpu,
			       struct x86_instruction_info *info,
			       enum x86_intercept_stage stage,
			       struct x86_exception *exception)
{
	struct vcpu_svm *svm = to_svm(vcpu);
	int vmexit, ret = X86EMUL_CONTINUE;
	struct __x86_intercept icpt_info;
	struct vmcb *vmcb = svm->vmcb;

	if (info->intercept >= ARRAY_SIZE(x86_intercept_map))
		goto out;

	icpt_info = x86_intercept_map[info->intercept];

	if (stage != icpt_info.stage)
		goto out;

	switch (icpt_info.exit_code) {
	case SVM_EXIT_READ_CR0:
		if (info->intercept == x86_intercept_cr_read)
			icpt_info.exit_code += info->modrm_reg;
		break;
	case SVM_EXIT_WRITE_CR0: {
		unsigned long cr0, val;
		u64 intercept;

		if (info->intercept == x86_intercept_cr_write)
			icpt_info.exit_code += info->modrm_reg;

		if (icpt_info.exit_code != SVM_EXIT_WRITE_CR0 ||
		    info->intercept == x86_intercept_clts)
			break;

		intercept = svm->nested.intercept;

		if (!(intercept & (1ULL << INTERCEPT_SELECTIVE_CR0)))
			break;

		cr0 = vcpu->arch.cr0 & ~SVM_CR0_SELECTIVE_MASK;
		val = info->src_val  & ~SVM_CR0_SELECTIVE_MASK;

		if (info->intercept == x86_intercept_lmsw) {
			cr0 &= 0xfUL;
			val &= 0xfUL;
			/* lmsw can't clear PE - catch this here */
			if (cr0 & X86_CR0_PE)
				val |= X86_CR0_PE;
		}

		if (cr0 ^ val)
			icpt_info.exit_code = SVM_EXIT_CR0_SEL_WRITE;

		break;
	}
	case SVM_EXIT_READ_DR0:
	case SVM_EXIT_WRITE_DR0:
		icpt_info.exit_code += info->modrm_reg;
		break;
	case SVM_EXIT_MSR:
		if (info->intercept == x86_intercept_wrmsr)
			vmcb->control.exit_info_1 = 1;
		else
			vmcb->control.exit_info_1 = 0;
		break;
	case SVM_EXIT_PAUSE:
		/*
		 * We get this for NOP only, but pause
		 * is rep not, check this here
		 */
		if (info->rep_prefix != REPE_PREFIX)
			goto out;
		break;
	case SVM_EXIT_IOIO: {
		u64 exit_info;
		u32 bytes;

		if (info->intercept == x86_intercept_in ||
		    info->intercept == x86_intercept_ins) {
			exit_info = ((info->src_val & 0xffff) << 16) |
				SVM_IOIO_TYPE_MASK;
			bytes = info->dst_bytes;
		} else {
			exit_info = (info->dst_val & 0xffff) << 16;
			bytes = info->src_bytes;
		}

		if (info->intercept == x86_intercept_outs ||
		    info->intercept == x86_intercept_ins)
			exit_info |= SVM_IOIO_STR_MASK;

		if (info->rep_prefix)
			exit_info |= SVM_IOIO_REP_MASK;

		bytes = min(bytes, 4u);

		exit_info |= bytes << SVM_IOIO_SIZE_SHIFT;

		exit_info |= (u32)info->ad_bytes << (SVM_IOIO_ASIZE_SHIFT - 1);

		vmcb->control.exit_info_1 = exit_info;
		vmcb->control.exit_info_2 = info->next_rip;

		break;
	}
	default:
		break;
	}

	/* TODO: Advertise NRIPS to guest hypervisor unconditionally */
	if (static_cpu_has(X86_FEATURE_NRIPS))
		vmcb->control.next_rip  = info->next_rip;
	vmcb->control.exit_code = icpt_info.exit_code;
	vmexit = nested_svm_exit_handled(svm);

	ret = (vmexit == NESTED_EXIT_DONE) ? X86EMUL_INTERCEPTED
					   : X86EMUL_CONTINUE;

out:
	return ret;
}