static inline bool is_sysrq_oom(struct oom_control *oc)
{
	return oc->order == -1;
}