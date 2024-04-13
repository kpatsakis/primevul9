term_fg_color(int n)
{
    // Use "AF" termcap entry if present, "Sf" entry otherwise
    if (*T_CAF)
	term_color(T_CAF, n);
    else if (*T_CSF)
	term_color(T_CSF, n);
}