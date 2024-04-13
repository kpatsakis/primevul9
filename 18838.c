term_cursor_color(char_u *color)
{
    if (*T_CSC != NUL)
    {
	out_str(T_CSC);		// set cursor color start
	out_str_nf(color);
	out_str(T_CEC);		// set cursor color end
	out_flush();
    }
}