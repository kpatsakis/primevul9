static __inline__ struct atalk_route *atalk_get_route_idx(loff_t pos)
{
	struct atalk_route *r;

	for (r = atalk_routes; pos && r; r = r->next)
		--pos;

	return r;
}