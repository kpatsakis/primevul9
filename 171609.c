ldns_tsig_keydata_clone(const ldns_tsig_credentials *tc)
{
	return strdup(tc->keydata);
}