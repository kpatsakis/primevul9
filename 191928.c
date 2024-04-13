parse_format(FormatNode *node, char *str, const KeyWord *kw,
			 const KeySuffix *suf, const int *index, int ver, NUMDesc *Num)
{
	const KeySuffix *s;
	FormatNode *n;
	int			node_set = 0,
				suffix,
				last = 0;

#ifdef DEBUG_TO_FROM_CHAR
	elog(DEBUG_elog_output, "to_char/number(): run parser");
#endif

	n = node;

	while (*str)
	{
		suffix = 0;

		/*
		 * Prefix
		 */
		if (ver == DCH_TYPE && (s = suff_search(str, suf, SUFFTYPE_PREFIX)) != NULL)
		{
			suffix |= s->id;
			if (s->len)
				str += s->len;
		}

		/*
		 * Keyword
		 */
		if (*str && (n->key = index_seq_search(str, kw, index)) != NULL)
		{
			n->type = NODE_TYPE_ACTION;
			n->suffix = 0;
			node_set = 1;
			if (n->key->len)
				str += n->key->len;

			/*
			 * NUM version: Prepare global NUMDesc struct
			 */
			if (ver == NUM_TYPE)
				NUMDesc_prepare(Num, n);

			/*
			 * Postfix
			 */
			if (ver == DCH_TYPE && *str && (s = suff_search(str, suf, SUFFTYPE_POSTFIX)) != NULL)
			{
				suffix |= s->id;
				if (s->len)
					str += s->len;
			}
		}
		else if (*str)
		{
			/*
			 * Special characters '\' and '"'
			 */
			if (*str == '"' && last != '\\')
			{
				int			x = 0;

				while (*(++str))
				{
					if (*str == '"' && x != '\\')
					{
						str++;
						break;
					}
					else if (*str == '\\' && x != '\\')
					{
						x = '\\';
						continue;
					}
					n->type = NODE_TYPE_CHAR;
					n->character = *str;
					n->key = NULL;
					n->suffix = 0;
					++n;
					x = *str;
				}
				node_set = 0;
				suffix = 0;
				last = 0;
			}
			else if (*str && *str == '\\' && last != '\\' && *(str + 1) == '"')
			{
				last = *str;
				str++;
			}
			else if (*str)
			{
				n->type = NODE_TYPE_CHAR;
				n->character = *str;
				n->key = NULL;
				node_set = 1;
				last = 0;
				str++;
			}
		}

		/* end */
		if (node_set)
		{
			if (n->type == NODE_TYPE_ACTION)
				n->suffix = suffix;
			++n;

			n->suffix = 0;
			node_set = 0;
		}
	}

	n->type = NODE_TYPE_END;
	n->suffix = 0;
	return;
}