inline static int is_type_printable(int type)
{
	if (type == GNUTLS_SAN_DNSNAME || type == GNUTLS_SAN_RFC822NAME ||
	    type == GNUTLS_SAN_URI || type == GNUTLS_SAN_OTHERNAME_XMPP ||
	    type == GNUTLS_SAN_OTHERNAME)
		return 1;
	else
		return 0;
}