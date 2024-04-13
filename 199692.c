input_osc_colour_reply(struct input_ctx *ictx, u_int n, int c)
{
    u_char	 r, g, b;
    const char	*end;

    if (c == 8 || (~c & COLOUR_FLAG_RGB))
	    return;
    colour_split_rgb(c, &r, &g, &b);

    if (ictx->input_end == INPUT_END_BEL)
	    end = "\007";
    else
	    end = "\033\\";
    input_reply(ictx, "\033]%u;rgb:%02hhx/%02hhx/%02hhx%s", n, r, g, b, end);
}