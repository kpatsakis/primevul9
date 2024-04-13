ns_client_checkacl(ns_client_t *client, isc_sockaddr_t *sockaddr,
		   const char *opname, dns_acl_t *acl,
		   bool default_allow, int log_level)
{
	isc_result_t result;
	isc_netaddr_t netaddr;

	if (sockaddr != NULL)
		isc_netaddr_fromsockaddr(&netaddr, sockaddr);

	result = ns_client_checkaclsilent(client, sockaddr ? &netaddr : NULL,
					  acl, default_allow);

	if (result == ISC_R_SUCCESS)
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "%s approved", opname);
	else
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT,
			      log_level, "%s denied", opname);
	return (result);
}