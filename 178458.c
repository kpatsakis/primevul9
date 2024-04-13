init_class_tab(void)
{
    int		i;
    static int	done = FALSE;

    if (done)
	return;

    for (i = 0; i < 256; ++i)
    {
	if (i >= '0' && i <= '7')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_OCTAL + RI_WORD;
	else if (i >= '8' && i <= '9')
	    class_tab[i] = RI_DIGIT + RI_HEX + RI_WORD;
	else if (i >= 'a' && i <= 'f')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	else if (i >= 'g' && i <= 'z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_LOWER;
	else if (i >= 'A' && i <= 'F')
	    class_tab[i] = RI_HEX + RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	else if (i >= 'G' && i <= 'Z')
	    class_tab[i] = RI_WORD + RI_HEAD + RI_ALPHA + RI_UPPER;
	else if (i == '_')
	    class_tab[i] = RI_WORD + RI_HEAD;
	else
	    class_tab[i] = 0;
    }
    class_tab[' '] |= RI_WHITE;
    class_tab['\t'] |= RI_WHITE;
    done = TRUE;
}