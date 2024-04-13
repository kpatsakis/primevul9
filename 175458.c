static inline bool htab_use_raw_lock(const struct bpf_htab *htab)
{
	return (!IS_ENABLED(CONFIG_PREEMPT_RT) || htab_is_prealloc(htab));
}