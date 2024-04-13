term_bg_color(int n)
{
    // Use "AB" termcap entry if present, "Sb" entry otherwise
    if (*T_CAB)
	term_color(T_CAB, n);
    else if (*T_CSB)
	term_color(T_CSB, n);
}