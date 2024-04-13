static u64 get_data_src(struct perf_event *event, u64 aux)
{
	u64 val = PERF_MEM_NA;
	int fl = event->hw.flags;
	bool fst = fl & (PERF_X86_EVENT_PEBS_ST | PERF_X86_EVENT_PEBS_HSW_PREC);

	if (fl & PERF_X86_EVENT_PEBS_LDLAT)
		val = load_latency_data(aux);
	else if (fl & PERF_X86_EVENT_PEBS_STLAT)
		val = store_latency_data(aux);
	else if (fst && (fl & PERF_X86_EVENT_PEBS_HSW_PREC))
		val = precise_datala_hsw(event, aux);
	else if (fst)
		val = precise_store_data(aux);
	return val;
}