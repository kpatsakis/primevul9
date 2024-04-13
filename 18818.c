term_cursor_right(int i)
{
    OUT_STR(tgoto((char *)T_CRI, 0, i));
}