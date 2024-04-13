term_get_fg_color(char_u *r, char_u *g, char_u *b)
{
    if (rfg_status.tr_progress == STATUS_GOT)
    {
	*r = fg_r;
	*g = fg_g;
	*b = fg_b;
    }
}