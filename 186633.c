create_vba_project(int record_count, const char *dir, struct uniq *U)
{
	vba_project_t *ret;

	ret = (vba_project_t *) cli_malloc(sizeof(struct vba_project_tag));

	if(ret == NULL)
		return NULL;

	ret->name = (char **)cli_malloc(sizeof(char *) * record_count);
	ret->colls = (uint32_t *)cli_malloc(sizeof(uint32_t) * record_count);
	ret->dir = cli_strdup(dir);
	ret->offset = (uint32_t *)cli_malloc (sizeof(uint32_t) * record_count);

	if((ret->name == NULL) || (ret->dir == NULL) || (ret->offset == NULL)) {
		if(ret->dir)
			free(ret->dir);
		if(ret->colls)
			free(ret->colls);
		if(ret->name)
			free(ret->name);
		if(ret->offset)
			free(ret->offset);
		free(ret);
		return NULL;
	}
	ret->count = record_count;
	ret->U = U;

	return ret;
}