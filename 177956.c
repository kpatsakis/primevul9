bjc_put_print_method_short(gp_file *file, char color)
{
    bjc_put_command(file, 'c', 1);
    gp_fputc(color, file);
}