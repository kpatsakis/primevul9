static inline bool xfrm_policy_mark_match(const struct xfrm_mark *mark,
					  struct xfrm_policy *pol)
{
	return mark->v == pol->mark.v && mark->m == pol->mark.m;
}