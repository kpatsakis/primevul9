PHPAPI const ps_serializer *_php_find_ps_serializer(char *name) /* {{{ */
{
	const ps_serializer *ret = NULL;
	const ps_serializer *mod;

	for (mod = ps_serializers; mod->name; mod++) {
		if (!strcasecmp(name, mod->name)) {
			ret = mod;
			break;
		}
	}
	return ret;
}