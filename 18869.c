term_color(char_u *s, int n)
{
    char	buf[20];
    int		i = *s == CSI ? 1 : 2;
		// index in s[] just after <Esc>[ or CSI

    // Special handling of 16 colors, because termcap can't handle it
    // Also accept "\e[3%dm" for TERMINFO, it is sometimes used
    // Also accept CSI instead of <Esc>[
    if (n >= 8 && t_colors >= 16
	      && ((s[0] == ESC && s[1] == '[')
#if defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		  || (s[0] == ESC && s[1] == '|')
#endif
		  || (s[0] == CSI && (i = 1) == 1))
	      && s[i] != NUL
	      && (STRCMP(s + i + 1, "%p1%dm") == 0
		  || STRCMP(s + i + 1, "%dm") == 0)
	      && (s[i] == '3' || s[i] == '4'))
    {
#ifdef TERMINFO
	char *format = "%s%s%%p1%%dm";
#else
	char *format = "%s%s%%dm";
#endif
	char *lead = i == 2 ? (
#if defined(FEAT_VTP) && defined(FEAT_TERMGUICOLORS)
		    s[1] == '|' ? "\033|" :
#endif
		    "\033[") : "\233";
	char *tail = s[i] == '3' ? (n >= 16 ? "38;5;" : "9")
				 : (n >= 16 ? "48;5;" : "10");

	sprintf(buf, format, lead, tail);
	OUT_STR(tgoto(buf, 0, n >= 16 ? n : n - 8));
    }
    else
	OUT_STR(tgoto((char *)s, 0, n));
}