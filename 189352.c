langmap_adjust_mb(int c)
{
    langmap_entry_T *entries = (langmap_entry_T *)(langmap_mapga.ga_data);
    int a = 0;
    int b = langmap_mapga.ga_len;

    while (a != b)
    {
	int i = (a + b) / 2;
	int d = entries[i].from - c;

	if (d == 0)
	    return entries[i].to;  /* found matching entry */
	if (d < 0)
	    a = i + 1;
	else
	    b = i;
    }
    return c;  /* no entry found, return "c" unmodified */
}