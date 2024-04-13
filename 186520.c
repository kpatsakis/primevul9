find_first_blank(pos_T *posp)
{
    int	    c;

    while (decl(posp) != -1)
    {
	c = gchar_pos(posp);
	if (!VIM_ISWHITE(c))
	{
	    incl(posp);
	    break;
	}
    }
}