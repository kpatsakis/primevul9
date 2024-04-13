term_delete_lines(int line_count)
{
    OUT_STR(tgoto((char *)T_CDL, 0, line_count));
}