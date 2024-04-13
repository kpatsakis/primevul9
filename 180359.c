static inline bool is_memcg_oom(struct oom_control *oc)
{
	return oc->memcg != NULL;
}