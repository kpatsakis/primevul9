set_chars_option(char_u **varp)
{
    int		round, i, len, entries;
    char_u	*p, *s;
    int		c1, c2 = 0;
    struct charstab
    {
	int	*cp;
	char	*name;
    };
#if defined(FEAT_WINDOWS) || defined(FEAT_FOLDING)
    static struct charstab filltab[] =
    {
	{&fill_stl,	"stl"},
	{&fill_stlnc,	"stlnc"},
	{&fill_vert,	"vert"},
	{&fill_fold,	"fold"},
	{&fill_diff,	"diff"},
    };
#endif
    static struct charstab lcstab[] =
    {
	{&lcs_eol,	"eol"},
	{&lcs_ext,	"extends"},
	{&lcs_nbsp,	"nbsp"},
	{&lcs_prec,	"precedes"},
	{&lcs_space,	"space"},
	{&lcs_tab2,	"tab"},
	{&lcs_trail,	"trail"},
#ifdef FEAT_CONCEAL
	{&lcs_conceal,	"conceal"},
#else
	{NULL,		"conceal"},
#endif
    };
    struct charstab *tab;

#if defined(FEAT_WINDOWS) || defined(FEAT_FOLDING)
    if (varp == &p_lcs)
#endif
    {
	tab = lcstab;
	entries = sizeof(lcstab) / sizeof(struct charstab);
    }
#if defined(FEAT_WINDOWS) || defined(FEAT_FOLDING)
    else
    {
	tab = filltab;
	entries = sizeof(filltab) / sizeof(struct charstab);
    }
#endif

    /* first round: check for valid value, second round: assign values */
    for (round = 0; round <= 1; ++round)
    {
	if (round > 0)
	{
	    /* After checking that the value is valid: set defaults: space for
	     * 'fillchars', NUL for 'listchars' */
	    for (i = 0; i < entries; ++i)
		if (tab[i].cp != NULL)
		    *(tab[i].cp) = (varp == &p_lcs ? NUL : ' ');
	    if (varp == &p_lcs)
		lcs_tab1 = NUL;
#if defined(FEAT_WINDOWS) || defined(FEAT_FOLDING)
	    else
		fill_diff = '-';
#endif
	}
	p = *varp;
	while (*p)
	{
	    for (i = 0; i < entries; ++i)
	    {
		len = (int)STRLEN(tab[i].name);
		if (STRNCMP(p, tab[i].name, len) == 0
			&& p[len] == ':'
			&& p[len + 1] != NUL)
		{
		    s = p + len + 1;
#ifdef FEAT_MBYTE
		    c1 = mb_ptr2char_adv(&s);
		    if (mb_char2cells(c1) > 1)
			continue;
#else
		    c1 = *s++;
#endif
		    if (tab[i].cp == &lcs_tab2)
		    {
			if (*s == NUL)
			    continue;
#ifdef FEAT_MBYTE
			c2 = mb_ptr2char_adv(&s);
			if (mb_char2cells(c2) > 1)
			    continue;
#else
			c2 = *s++;
#endif
		    }
		    if (*s == ',' || *s == NUL)
		    {
			if (round)
			{
			    if (tab[i].cp == &lcs_tab2)
			    {
				lcs_tab1 = c1;
				lcs_tab2 = c2;
			    }
			    else if (tab[i].cp != NULL)
				*(tab[i].cp) = c1;

			}
			p = s;
			break;
		    }
		}
	    }

	    if (i == entries)
		return e_invarg;
	    if (*p == ',')
		++p;
	}
    }

    return NULL;	/* no error */
}