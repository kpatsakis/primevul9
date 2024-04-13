get_term_entries(int *height, int *width)
{
    static struct {
		    enum SpecialKey dest; // index in term_strings[]
		    char *name;		  // termcap name for string
		  } string_names[] =
		    {	{KS_CE, "ce"}, {KS_AL, "al"}, {KS_CAL,"AL"},
			{KS_DL, "dl"}, {KS_CDL,"DL"}, {KS_CS, "cs"},
			{KS_CL, "cl"}, {KS_CD, "cd"},
			{KS_VI, "vi"}, {KS_VE, "ve"}, {KS_MB, "mb"},
			{KS_ME, "me"}, {KS_MR, "mr"},
			{KS_MD, "md"}, {KS_SE, "se"}, {KS_SO, "so"},
			{KS_CZH,"ZH"}, {KS_CZR,"ZR"}, {KS_UE, "ue"},
			{KS_US, "us"}, {KS_UCE, "Ce"}, {KS_UCS, "Cs"},
			{KS_STE,"Te"}, {KS_STS,"Ts"},
			{KS_CM, "cm"}, {KS_SR, "sr"},
			{KS_CRI,"RI"}, {KS_VB, "vb"}, {KS_KS, "ks"},
			{KS_KE, "ke"}, {KS_TI, "ti"}, {KS_TE, "te"},
			{KS_CTI, "TI"}, {KS_CTE, "TE"},
			{KS_BC, "bc"}, {KS_CSB,"Sb"}, {KS_CSF,"Sf"},
			{KS_CAB,"AB"}, {KS_CAF,"AF"}, {KS_CAU,"AU"},
			{KS_LE, "le"},
			{KS_ND, "nd"}, {KS_OP, "op"}, {KS_CRV, "RV"},
			{KS_VS, "vs"}, {KS_CVS, "VS"},
			{KS_CIS, "IS"}, {KS_CIE, "IE"},
			{KS_CSC, "SC"}, {KS_CEC, "EC"},
			{KS_TS, "ts"}, {KS_FS, "fs"},
			{KS_CWP, "WP"}, {KS_CWS, "WS"},
			{KS_CSI, "SI"}, {KS_CEI, "EI"},
			{KS_U7, "u7"}, {KS_RFG, "RF"}, {KS_RBG, "RB"},
			{KS_8F, "8f"}, {KS_8B, "8b"}, {KS_8U, "8u"},
			{KS_CBE, "BE"}, {KS_CBD, "BD"},
			{KS_CPS, "PS"}, {KS_CPE, "PE"},
			{KS_CST, "ST"}, {KS_CRT, "RT"},
			{KS_SSI, "Si"}, {KS_SRI, "Ri"},
			{(enum SpecialKey)0, NULL}
		    };
    int		    i;
    char_u	    *p;
    static char_u   tstrbuf[TBUFSZ];
    char_u	    *tp = tstrbuf;

    /*
     * get output strings
     */
    for (i = 0; string_names[i].name != NULL; ++i)
    {
	if (TERM_STR(string_names[i].dest) == NULL
			     || TERM_STR(string_names[i].dest) == empty_option)
	{
	    TERM_STR(string_names[i].dest) = TGETSTR(string_names[i].name, &tp);
#ifdef FEAT_EVAL
	    set_term_option_sctx_idx(string_names[i].name, -1);
#endif
	}
    }

    // tgetflag() returns 1 if the flag is present, 0 if not and
    // possibly -1 if the flag doesn't exist.
    if ((T_MS == NULL || T_MS == empty_option) && tgetflag("ms") > 0)
	T_MS = (char_u *)"y";
    if ((T_XS == NULL || T_XS == empty_option) && tgetflag("xs") > 0)
	T_XS = (char_u *)"y";
    if ((T_XN == NULL || T_XN == empty_option) && tgetflag("xn") > 0)
	T_XN = (char_u *)"y";
    if ((T_DB == NULL || T_DB == empty_option) && tgetflag("db") > 0)
	T_DB = (char_u *)"y";
    if ((T_DA == NULL || T_DA == empty_option) && tgetflag("da") > 0)
	T_DA = (char_u *)"y";
    if ((T_UT == NULL || T_UT == empty_option) && tgetflag("ut") > 0)
	T_UT = (char_u *)"y";

    /*
     * get key codes
     */
    for (i = 0; key_names[i] != NULL; ++i)
	if (find_termcode((char_u *)key_names[i]) == NULL)
	{
	    p = TGETSTR(key_names[i], &tp);
	    // if cursor-left == backspace, ignore it (televideo 925)
	    if (p != NULL
		    && (*p != Ctrl_H
			|| key_names[i][0] != 'k'
			|| key_names[i][1] != 'l'))
		add_termcode((char_u *)key_names[i], p, FALSE);
	}

    if (*height == 0)
	*height = tgetnum("li");
    if (*width == 0)
	*width = tgetnum("co");

    /*
     * Get number of colors (if not done already).
     */
    if (TERM_STR(KS_CCO) == NULL || TERM_STR(KS_CCO) == empty_option)
    {
	set_color_count(tgetnum("Co"));
#ifdef FEAT_EVAL
	set_term_option_sctx_idx("Co", -1);
#endif
    }

# ifndef hpux
    BC = (char *)TGETSTR("bc", &tp);
    UP = (char *)TGETSTR("up", &tp);
    p = TGETSTR("pc", &tp);
    if (p)
	PC = *p;
# endif
}