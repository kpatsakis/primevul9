PHP_FUNCTION(dns_get_record)
{
	char *hostname;
	size_t hostname_len;
	long type_param = PHP_DNS_ANY;
	zval *authns = NULL, *addtl = NULL;
	int type_to_fetch;
#if defined(HAVE_DNS_SEARCH)
	struct sockaddr_storage from;
	uint32_t fromsize = sizeof(from);
	dns_handle_t handle;
#elif defined(HAVE_RES_NSEARCH)
	struct __res_state state;
	struct __res_state *handle = &state;
#endif
	HEADER *hp;
	querybuf answer;
	u_char *cp = NULL, *end = NULL;
	int n, qd, an, ns = 0, ar = 0;
	int type, first_query = 1, store_results = 1;
	zend_bool raw = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|lz/!z/!b",
			&hostname, &hostname_len, &type_param, &authns, &addtl, &raw) == FAILURE) {
		return;
	}

	if (authns) {
		zval_dtor(authns);
		array_init(authns);
	}
	if (addtl) {
		zval_dtor(addtl);
		array_init(addtl);
	}

	if (!raw) {
		if ((type_param & ~PHP_DNS_ALL) && (type_param != PHP_DNS_ANY)) {
			php_error_docref(NULL, E_WARNING, "Type '%ld' not supported", type_param);
			RETURN_FALSE;
		}
	} else {
		if ((type_param < 1) || (type_param > 0xFFFF)) {
			php_error_docref(NULL, E_WARNING,
				"Numeric DNS record type must be between 1 and 65535, '%ld' given", type_param);
			RETURN_FALSE;
		}
	}

	/* Initialize the return array */
	array_init(return_value);

	/* - We emulate an or'ed type mask by querying type by type. (Steps 0 - NUMTYPES-1 )
	 *   If additional info is wanted we check again with DNS_T_ANY (step NUMTYPES / NUMTYPES+1 )
	 *   store_results is used to skip storing the results retrieved in step
	 *   NUMTYPES+1 when results were already fetched.
	 * - In case of PHP_DNS_ANY we use the directly fetch DNS_T_ANY. (step NUMTYPES+1 )
	 * - In case of raw mode, we query only the requestd type instead of looping type by type
	 *   before going with the additional info stuff.
	 */

	if (raw) {
		type = -1;
	} else if (type_param == PHP_DNS_ANY) {
		type = PHP_DNS_NUM_TYPES + 1;
	} else {
		type = 0;
	}

	for ( ;
		type < (addtl ? (PHP_DNS_NUM_TYPES + 2) : PHP_DNS_NUM_TYPES) || first_query;
		type++
	) {
		first_query = 0;
		switch (type) {
			case -1: /* raw */
				type_to_fetch = type_param;
				/* skip over the rest and go directly to additional records */
				type = PHP_DNS_NUM_TYPES - 1;
				break;
			case 0:
				type_to_fetch = type_param&PHP_DNS_A     ? DNS_T_A     : 0;
				break;
			case 1:
				type_to_fetch = type_param&PHP_DNS_NS    ? DNS_T_NS    : 0;
				break;
			case 2:
				type_to_fetch = type_param&PHP_DNS_CNAME ? DNS_T_CNAME : 0;
				break;
			case 3:
				type_to_fetch = type_param&PHP_DNS_SOA   ? DNS_T_SOA   : 0;
				break;
			case 4:
				type_to_fetch = type_param&PHP_DNS_PTR   ? DNS_T_PTR   : 0;
				break;
			case 5:
				type_to_fetch = type_param&PHP_DNS_HINFO ? DNS_T_HINFO : 0;
				break;
			case 6:
				type_to_fetch = type_param&PHP_DNS_MX    ? DNS_T_MX    : 0;
				break;
			case 7:
				type_to_fetch = type_param&PHP_DNS_TXT   ? DNS_T_TXT   : 0;
				break;
			case 8:
				type_to_fetch = type_param&PHP_DNS_AAAA	 ? DNS_T_AAAA  : 0;
				break;
			case 9:
				type_to_fetch = type_param&PHP_DNS_SRV   ? DNS_T_SRV   : 0;
				break;
			case 10:
				type_to_fetch = type_param&PHP_DNS_NAPTR ? DNS_T_NAPTR : 0;
				break;
			case 11:
				type_to_fetch = type_param&PHP_DNS_A6	 ? DNS_T_A6 : 0;
				break;
			case 12:
				type_to_fetch = type_param&PHP_DNS_CAA ? DNS_T_CAA : 0;
				break;
			case PHP_DNS_NUM_TYPES:
				store_results = 0;
				continue;
			default:
			case (PHP_DNS_NUM_TYPES + 1):
				type_to_fetch = DNS_T_ANY;
				break;
		}

		if (type_to_fetch) {
#if defined(HAVE_DNS_SEARCH)
			handle = dns_open(NULL);
			if (handle == NULL) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}
#elif defined(HAVE_RES_NSEARCH)
		    memset(&state, 0, sizeof(state));
		    if (res_ninit(handle)) {
		    	zval_dtor(return_value);
				RETURN_FALSE;
			}
#else
			res_init();
#endif

			n = php_dns_search(handle, hostname, C_IN, type_to_fetch, answer.qb2, sizeof answer);

			if (n < 0) {
				php_dns_free_handle(handle);
				switch (h_errno) {
					case NO_DATA:
					case HOST_NOT_FOUND:
						continue;

					case NO_RECOVERY:
						php_error_docref(NULL, E_WARNING, "An unexpected server failure occurred.");
						break;

					case TRY_AGAIN:
						php_error_docref(NULL, E_WARNING, "A temporary server error occurred.");
						break;

					default:
						php_error_docref(NULL, E_WARNING, "DNS Query failed");
				}
				zval_dtor(return_value);
				RETURN_FALSE;
			}

			cp = answer.qb2 + HFIXEDSZ;
			end = answer.qb2 + n;
			hp = (HEADER *)&answer;
			qd = ntohs(hp->qdcount);
			an = ntohs(hp->ancount);
			ns = ntohs(hp->nscount);
			ar = ntohs(hp->arcount);

			/* Skip QD entries, they're only used by dn_expand later on */
			while (qd-- > 0) {
				n = dn_skipname(cp, end);
				if (n < 0) {
					php_error_docref(NULL, E_WARNING, "Unable to parse DNS data received");
					zval_dtor(return_value);
					php_dns_free_handle(handle);
					RETURN_FALSE;
				}
				cp += n + QFIXEDSZ;
			}

			/* YAY! Our real answers! */
			while (an-- && cp && cp < end) {
				zval retval;

				cp = php_parserr(cp, end, &answer, type_to_fetch, store_results, raw, &retval);
				if (Z_TYPE(retval) != IS_UNDEF && store_results) {
					add_next_index_zval(return_value, &retval);
				}
			}

			if (authns || addtl) {
				/* List of Authoritative Name Servers
				 * Process when only requesting addtl so that we can skip through the section
				 */
				while (ns-- > 0 && cp && cp < end) {
					zval retval;

					cp = php_parserr(cp, end, &answer, DNS_T_ANY, authns != NULL, raw, &retval);
					if (Z_TYPE(retval) != IS_UNDEF) {
						add_next_index_zval(authns, &retval);
					}
				}
			}

			if (addtl) {
				/* Additional records associated with authoritative name servers */
				while (ar-- > 0 && cp && cp < end) {
					zval retval;

					cp = php_parserr(cp, end, &answer, DNS_T_ANY, 1, raw, &retval);
					if (Z_TYPE(retval) != IS_UNDEF) {
						add_next_index_zval(addtl, &retval);
					}
				}
			}
			php_dns_free_handle(handle);
		}
	}
}