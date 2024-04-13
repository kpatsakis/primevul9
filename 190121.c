static int role_regs_to_root_level(struct kvm_mmu_role_regs *regs)
{
	if (!____is_cr0_pg(regs))
		return 0;
	else if (____is_efer_lma(regs))
		return ____is_cr4_la57(regs) ? PT64_ROOT_5LEVEL :
					       PT64_ROOT_4LEVEL;
	else if (____is_cr4_pae(regs))
		return PT32E_ROOT_LEVEL;
	else
		return PT32_ROOT_LEVEL;
}