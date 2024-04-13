bjc_put_page_margins(gp_file *file, int length, int lm, int rm, int top)
{
    byte parms[4];

    parms[0] = length, parms[1] = lm, parms[2] = rm, parms[3] = top;
/*    count = 4;       */ /* could be 1..3 */
    bjc_put_command(file, 'g', 4);
    bjc_put_bytes(file, parms, 4);
}