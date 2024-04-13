 */
static zend_string* _php_rfc822_write_address(ADDRESS *addresslist)
{
	char address[MAILTMPLEN];
	smart_str ret = {0};
	RFC822BUFFER buf;

	buf.beg = address;
	buf.cur = buf.beg;
	buf.end = buf.beg + sizeof(address) - 1;
	buf.s = &ret;
	buf.f = _php_rfc822_soutr;
	rfc822_output_address_list(&buf, addresslist, 0, NULL);
	rfc822_output_flush(&buf);
	smart_str_0(&ret);
	return ret.s;