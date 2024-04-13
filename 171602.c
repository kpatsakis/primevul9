ldns_tsig_keyname_clone(const ldns_tsig_credentials *tc)
{
	return strdup(tc->keyname);
}