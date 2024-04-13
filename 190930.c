 */
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist)
{
	char address[SENDBUFLEN];

	if (_php_imap_address_size(addresslist) >= SENDBUFLEN) {
		zend_throw_error(NULL, "Address buffer overflow");
		return NULL;
	}
	address[0] = 0;
	rfc822_write_address(address, addresslist);
	return zend_string_init(address, strlen(address), 0);