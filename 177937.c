bjc_put_move_lines_unit(gp_file *file, int unit)
{
    bjc_put_command(file, 'o', 2);
    bjc_put_hi_lo(file, unit);
}