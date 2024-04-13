static inline void ipv6_store_devconf(struct ipv6_devconf *cnf,
				__s32 *array, int bytes)
{
	BUG_ON(bytes < (DEVCONF_MAX * 4));

	memset(array, 0, bytes);
	array[DEVCONF_FORWARDING] = cnf->forwarding;
	array[DEVCONF_HOPLIMIT] = cnf->hop_limit;
	array[DEVCONF_MTU6] = cnf->mtu6;
	array[DEVCONF_ACCEPT_RA] = cnf->accept_ra;
	array[DEVCONF_ACCEPT_REDIRECTS] = cnf->accept_redirects;
	array[DEVCONF_AUTOCONF] = cnf->autoconf;
	array[DEVCONF_DAD_TRANSMITS] = cnf->dad_transmits;
	array[DEVCONF_RTR_SOLICITS] = cnf->rtr_solicits;
	array[DEVCONF_RTR_SOLICIT_INTERVAL] =
		jiffies_to_msecs(cnf->rtr_solicit_interval);
	array[DEVCONF_RTR_SOLICIT_DELAY] =
		jiffies_to_msecs(cnf->rtr_solicit_delay);
	array[DEVCONF_FORCE_MLD_VERSION] = cnf->force_mld_version;
#ifdef CONFIG_IPV6_PRIVACY
	array[DEVCONF_USE_TEMPADDR] = cnf->use_tempaddr;
	array[DEVCONF_TEMP_VALID_LFT] = cnf->temp_valid_lft;
	array[DEVCONF_TEMP_PREFERED_LFT] = cnf->temp_prefered_lft;
	array[DEVCONF_REGEN_MAX_RETRY] = cnf->regen_max_retry;
	array[DEVCONF_MAX_DESYNC_FACTOR] = cnf->max_desync_factor;
#endif
	array[DEVCONF_MAX_ADDRESSES] = cnf->max_addresses;
	array[DEVCONF_ACCEPT_RA_DEFRTR] = cnf->accept_ra_defrtr;
	array[DEVCONF_ACCEPT_RA_PINFO] = cnf->accept_ra_pinfo;
#ifdef CONFIG_IPV6_ROUTER_PREF
	array[DEVCONF_ACCEPT_RA_RTR_PREF] = cnf->accept_ra_rtr_pref;
	array[DEVCONF_RTR_PROBE_INTERVAL] =
		jiffies_to_msecs(cnf->rtr_probe_interval);
#ifdef CONFIG_IPV6_ROUTE_INFO
	array[DEVCONF_ACCEPT_RA_RT_INFO_MAX_PLEN] = cnf->accept_ra_rt_info_max_plen;
#endif
#endif
	array[DEVCONF_PROXY_NDP] = cnf->proxy_ndp;
	array[DEVCONF_ACCEPT_SOURCE_ROUTE] = cnf->accept_source_route;
#ifdef CONFIG_IPV6_OPTIMISTIC_DAD
	array[DEVCONF_OPTIMISTIC_DAD] = cnf->optimistic_dad;
#endif
#ifdef CONFIG_IPV6_MROUTE
	array[DEVCONF_MC_FORWARDING] = cnf->mc_forwarding;
#endif
	array[DEVCONF_DISABLE_IPV6] = cnf->disable_ipv6;
	array[DEVCONF_ACCEPT_DAD] = cnf->accept_dad;
	array[DEVCONF_FORCE_TLLAO] = cnf->force_tllao;
	array[DEVCONF_NDISC_NOTIFY] = cnf->ndisc_notify;
}