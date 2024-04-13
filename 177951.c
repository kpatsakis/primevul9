bjc_put_move_lines(gp_file *file, int lines)
{
    bjc_put_command(file, 'n', 2);
    bjc_put_hi_lo(file, lines);
}