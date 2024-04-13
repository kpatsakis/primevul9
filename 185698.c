normalize_cookie_domain (const char *domain)
{
	/* Trim any leading dot if present to transform the cookie
         * domain into a valid hostname.
         */
	if (domain != NULL && domain[0] == '.')
		return domain + 1;
	return domain;
}