term_get_bg_color(char_u *r, char_u *g, char_u *b)
{
    if (rbg_status.tr_progress == STATUS_GOT)
    {
	*r = bg_r;
	*g = bg_g;
	*b = bg_b;
    }
}