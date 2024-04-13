ath6kl_regd_find_country(u16 countryCode)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(allCountries); i++) {
		if (allCountries[i].countryCode == countryCode)
			return &allCountries[i];
	}

	return NULL;
}