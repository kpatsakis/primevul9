term_windgoto(int row, int col)
{
    OUT_STR(tgoto((char *)T_CM, col, row));
}