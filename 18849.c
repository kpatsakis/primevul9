term_ul_rgb_color(guicolor_T rgb)
{
# ifdef FEAT_TERMRESPONSE
    if (write_t_8u_state != OK)
	write_t_8u_state = MAYBE;
    else
# endif
	term_rgb_color(T_8U, rgb);
}