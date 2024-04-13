static void dump_ghcb(struct vcpu_svm *svm)
{
	struct ghcb *ghcb = svm->ghcb;
	unsigned int nbits;

	/* Re-use the dump_invalid_vmcb module parameter */
	if (!dump_invalid_vmcb) {
		pr_warn_ratelimited("set kvm_amd.dump_invalid_vmcb=1 to dump internal KVM state.\n");
		return;
	}

	nbits = sizeof(ghcb->save.valid_bitmap) * 8;

	pr_err("GHCB (GPA=%016llx):\n", svm->vmcb->control.ghcb_gpa);
	pr_err("%-20s%016llx is_valid: %u\n", "sw_exit_code",
	       ghcb->save.sw_exit_code, ghcb_sw_exit_code_is_valid(ghcb));
	pr_err("%-20s%016llx is_valid: %u\n", "sw_exit_info_1",
	       ghcb->save.sw_exit_info_1, ghcb_sw_exit_info_1_is_valid(ghcb));
	pr_err("%-20s%016llx is_valid: %u\n", "sw_exit_info_2",
	       ghcb->save.sw_exit_info_2, ghcb_sw_exit_info_2_is_valid(ghcb));
	pr_err("%-20s%016llx is_valid: %u\n", "sw_scratch",
	       ghcb->save.sw_scratch, ghcb_sw_scratch_is_valid(ghcb));
	pr_err("%-20s%*pb\n", "valid_bitmap", nbits, ghcb->save.valid_bitmap);
}