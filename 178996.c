static __inline__ struct atalk_iface *atalk_get_interface_idx(loff_t pos)
{
	struct atalk_iface *i;

	for (i = atalk_interfaces; pos && i; i = i->next)
		--pos;

	return i;
}