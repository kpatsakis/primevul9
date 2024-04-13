static union kvm_mmu_extended_role kvm_calc_mmu_role_ext(struct kvm_vcpu *vcpu,
							 struct kvm_mmu_role_regs *regs)
{
	union kvm_mmu_extended_role ext = {0};

	if (____is_cr0_pg(regs)) {
		ext.cr0_pg = 1;
		ext.cr4_pae = ____is_cr4_pae(regs);
		ext.cr4_smep = ____is_cr4_smep(regs);
		ext.cr4_smap = ____is_cr4_smap(regs);
		ext.cr4_pse = ____is_cr4_pse(regs);

		/* PKEY and LA57 are active iff long mode is active. */
		ext.cr4_pke = ____is_efer_lma(regs) && ____is_cr4_pke(regs);
		ext.cr4_la57 = ____is_efer_lma(regs) && ____is_cr4_la57(regs);
		ext.efer_lma = ____is_efer_lma(regs);
	}

	ext.valid = 1;

	return ext;
}