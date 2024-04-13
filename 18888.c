term_rgb_color(char_u *s, guicolor_T rgb)
{
#define MAX_COLOR_STR_LEN 100
    char	buf[MAX_COLOR_STR_LEN];

    if (*s == NUL)
	return;
    vim_snprintf(buf, MAX_COLOR_STR_LEN,
				  (char *)s, RED(rgb), GREEN(rgb), BLUE(rgb));
#ifdef FEAT_VTP
    if (use_wt())
    {
	out_flush();
	buf[1] = '[';
	vtp_printf(buf);
    }
    else
#endif
	OUT_STR(buf);
}