index_seq_search(char *str, const KeyWord *kw, const int *index)
{
	int			poz;

	if (!KeyWord_INDEX_FILTER(*str))
		return NULL;

	if ((poz = *(index + (*str - ' '))) > -1)
	{
		const KeyWord *k = kw + poz;

		do
		{
			if (strncmp(str, k->name, k->len) == 0)
				return k;
			k++;
			if (!k->name)
				return NULL;
		} while (*str == *k->name);
	}
	return NULL;
}