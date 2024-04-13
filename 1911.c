int nested_svm_exit_special(struct vcpu_svm *svm)
{
	u32 exit_code = svm->vmcb->control.exit_code;

	switch (exit_code) {
	case SVM_EXIT_INTR:
	case SVM_EXIT_NMI:
	case SVM_EXIT_NPF:
		return NESTED_EXIT_HOST;
	case SVM_EXIT_EXCP_BASE ... SVM_EXIT_EXCP_BASE + 0x1f: {
		u32 excp_bits = 1 << (exit_code - SVM_EXIT_EXCP_BASE);

		if (svm->vmcb01.ptr->control.intercepts[INTERCEPT_EXCEPTION] &
		    excp_bits)
			return NESTED_EXIT_HOST;
		else if (exit_code == SVM_EXIT_EXCP_BASE + PF_VECTOR &&
			 svm->vcpu.arch.apf.host_apf_flags)
			/* Trap async PF even if not shadowing */
			return NESTED_EXIT_HOST;
		break;
	}
	default:
		break;
	}

	return NESTED_EXIT_CONTINUE;
}