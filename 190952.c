 */
static zend_string* _php_imap_parse_address (ADDRESS *addresslist, zval *paddress)
{
	zend_string *fulladdress;
	ADDRESS *addresstmp;
	zval tmpvals;

	addresstmp = addresslist;

	fulladdress = _php_rfc822_write_address(addresstmp);

	addresstmp = addresslist;
	do {
		object_init(&tmpvals);
		if (addresstmp->personal) add_property_string(&tmpvals, "personal", addresstmp->personal);
		if (addresstmp->adl) add_property_string(&tmpvals, "adl", addresstmp->adl);
		if (addresstmp->mailbox) add_property_string(&tmpvals, "mailbox", addresstmp->mailbox);
		if (addresstmp->host) add_property_string(&tmpvals, "host", addresstmp->host);
		add_next_index_object(paddress, &tmpvals);
	} while ((addresstmp = addresstmp->next));
	return fulladdress;