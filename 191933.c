NUM_cache(int len, NUMDesc *Num, text *pars_str, bool *shouldFree)
{
	FormatNode *format = NULL;
	char	   *str;

	str = text_to_cstring(pars_str);

	/*
	 * Allocate new memory if format picture is bigger than static cache and
	 * not use cache (call parser always). This branches sets shouldFree to
	 * true, accordingly.
	 */
	if (len > NUM_CACHE_SIZE)
	{
		format = (FormatNode *) palloc((len + 1) * sizeof(FormatNode));

		*shouldFree = true;

		zeroize_NUM(Num);

		parse_format(format, str, NUM_keywords,
					 NULL, NUM_index, NUM_TYPE, Num);

		(format + len)->type = NODE_TYPE_END;	/* Paranoia? */
	}
	else
	{
		/*
		 * Use cache buffers
		 */
		NUMCacheEntry *ent;

		*shouldFree = false;

		if ((ent = NUM_cache_search(str)) == NULL)
		{
			ent = NUM_cache_getnew(str);

			/*
			 * Not in the cache, must run parser and save a new format-picture
			 * to the cache.
			 */
			parse_format(ent->format, str, NUM_keywords,
						 NULL, NUM_index, NUM_TYPE, &ent->Num);

			(ent->format + len)->type = NODE_TYPE_END;	/* Paranoia? */
		}

		format = ent->format;

		/*
		 * Copy cache to used struct
		 */
		Num->flag = ent->Num.flag;
		Num->lsign = ent->Num.lsign;
		Num->pre = ent->Num.pre;
		Num->post = ent->Num.post;
		Num->pre_lsign_num = ent->Num.pre_lsign_num;
		Num->need_locale = ent->Num.need_locale;
		Num->multi = ent->Num.multi;
		Num->zero_start = ent->Num.zero_start;
		Num->zero_end = ent->Num.zero_end;
	}

#ifdef DEBUG_TO_FROM_CHAR
	/* dump_node(format, len); */
	dump_index(NUM_keywords, NUM_index);
#endif

	pfree(str);
	return format;
}