bjc_put_extended_margins(gp_file *file, int length, int lm, int rm, int top)
{
    bjc_put_command(file, 'p', 8);
    bjc_put_hi_lo(file, length);
    bjc_put_hi_lo(file, lm);
    bjc_put_hi_lo(file, rm);
    bjc_put_hi_lo(file, top);
}