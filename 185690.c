incoming_cookie_is_third_party (SoupCookie *cookie, SoupURI *first_party)
{
	const char *normalized_cookie_domain;
	const char *cookie_base_domain;
	const char *first_party_base_domain;

	if (first_party == NULL || first_party->host == NULL)
		return TRUE;

	normalized_cookie_domain = normalize_cookie_domain (cookie->domain);
	cookie_base_domain = soup_tld_get_base_domain (normalized_cookie_domain, NULL);
	if (cookie_base_domain == NULL)
		cookie_base_domain = cookie->domain;

	first_party_base_domain = soup_tld_get_base_domain (first_party->host, NULL);
	if (first_party_base_domain == NULL)
		first_party_base_domain = first_party->host;
	return !soup_host_matches_host (cookie_base_domain, first_party_base_domain);
}