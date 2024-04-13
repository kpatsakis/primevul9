term_append_lines(int line_count)
{
    OUT_STR(tgoto((char *)T_CAL, 0, line_count));
}