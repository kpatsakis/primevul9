static void adaptive_pebs_record_size_update(void)
{
	struct cpu_hw_events *cpuc = this_cpu_ptr(&cpu_hw_events);
	u64 pebs_data_cfg = cpuc->pebs_data_cfg;
	int sz = sizeof(struct pebs_basic);

	if (pebs_data_cfg & PEBS_DATACFG_MEMINFO)
		sz += sizeof(struct pebs_meminfo);
	if (pebs_data_cfg & PEBS_DATACFG_GP)
		sz += sizeof(struct pebs_gprs);
	if (pebs_data_cfg & PEBS_DATACFG_XMMS)
		sz += sizeof(struct pebs_xmm);
	if (pebs_data_cfg & PEBS_DATACFG_LBRS)
		sz += x86_pmu.lbr_nr * sizeof(struct lbr_entry);

	cpuc->pebs_record_size = sz;
}