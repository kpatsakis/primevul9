static void sev_asid_free(struct kvm_sev_info *sev)
{
	struct svm_cpu_data *sd;
	int cpu;
	enum misc_res_type type;

	mutex_lock(&sev_bitmap_lock);

	__set_bit(sev->asid, sev_reclaim_asid_bitmap);

	for_each_possible_cpu(cpu) {
		sd = per_cpu(svm_data, cpu);
		sd->sev_vmcbs[sev->asid] = NULL;
	}

	mutex_unlock(&sev_bitmap_lock);

	type = sev->es_active ? MISC_CG_RES_SEV_ES : MISC_CG_RES_SEV;
	misc_cg_uncharge(type, sev->misc_cg, 1);
	put_misc_cg(sev->misc_cg);
	sev->misc_cg = NULL;
}