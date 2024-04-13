static int unix_instantiate(CONF_SECTION *conf, void **instance)
{
	struct unix_instance *inst;

	/*
	 *	Allocate room for the instance.
	 */
	inst = *instance = rad_malloc(sizeof(*inst));
	if (!inst) {
		return -1;
	}
	memset(inst, 0, sizeof(*inst));

	/*
	 *	Parse the configuration, failing if we can't do so.
	 */
	if (cf_section_parse(conf, inst, module_config) < 0) {
		unix_detach(inst);
		return -1;
	}

	/* FIXME - delay these until a group file has been read so we know
	 * groupcmp can actually do something */
	paircompare_register(PW_GROUP, PW_USER_NAME, groupcmp, NULL);
#ifdef PW_GROUP_NAME /* compat */
	paircompare_register(PW_GROUP_NAME, PW_USER_NAME, groupcmp, NULL);
#endif

#undef inst

	return 0;
}