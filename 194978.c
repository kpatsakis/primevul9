static void _php_dns_free_res(struct __res_state *res) { /* {{{ */
	int ns;
	for (ns = 0; ns < MAXNS; ns++) {
		if (res->_u._ext.nsaddrs[ns] != NULL) {
			free (res->_u._ext.nsaddrs[ns]);
			res->_u._ext.nsaddrs[ns] = NULL;
		}
	}
} /* }}} */