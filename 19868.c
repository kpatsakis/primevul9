static size_t ovs_nsh_key_attr_size(void)
{
	/* Whenever adding new OVS_NSH_KEY_ FIELDS, we should consider
	 * updating this function.
	 */
	return  nla_total_size(NSH_BASE_HDR_LEN) /* OVS_NSH_KEY_ATTR_BASE */
		/* OVS_NSH_KEY_ATTR_MD1 and OVS_NSH_KEY_ATTR_MD2 are
		 * mutually exclusive, so the bigger one can cover
		 * the small one.
		 */
		+ nla_total_size(NSH_CTX_HDRS_MAX_LEN);
}