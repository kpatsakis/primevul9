static zend_string *php_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr in;
	char *address;

	hp = php_network_gethostbyname(name);

	if (!hp || !*(hp->h_addr_list)) {
		return zend_string_init(name, strlen(name), 0);
	}

	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));

	address = inet_ntoa(in);
	return zend_string_init(address, strlen(address), 0);
}