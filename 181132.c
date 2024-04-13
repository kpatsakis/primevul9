ath6kl_get_regpair(u16 regdmn)
{
	int i;

	if (regdmn == NO_ENUMRD)
		return NULL;

	for (i = 0; i < ARRAY_SIZE(regDomainPairs); i++) {
		if (regDomainPairs[i].reg_domain == regdmn)
			return &regDomainPairs[i];
	}

	return NULL;
}