langmap_set_entry(int from, int to)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    int		    a = 0;
    int		    b = langmap_mapga.ga_len;

    /* Do a binary search for an existing entry. */
    while (a != b)
    {
	int i = (a + b) / 2;
	int d = entries[i].from - from;

	if (d == 0)
	{
	    entries[i].to = to;
	    return;
	}
	if (d < 0)
	    a = i + 1;
	else
	    b = i;
    }

    if (ga_grow(&langmap_mapga, 1) != OK)
	return;  /* out of memory */

    /* insert new entry at position "a" */
    entries = (langmap_entry_T *)(langmap_mapga.ga_data) + a;
    mch_memmove(entries + 1, entries,
			(langmap_mapga.ga_len - a) * sizeof(langmap_entry_T));
    ++langmap_mapga.ga_len;
    entries[0].from = from;
    entries[0].to = to;
}