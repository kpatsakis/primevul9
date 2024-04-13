static inline u64 intel_get_tsx_weight(u64 tsx_tuning)
{
	if (tsx_tuning) {
		union hsw_tsx_tuning tsx = { .value = tsx_tuning };
		return tsx.cycles_last_block;
	}
	return 0;
}