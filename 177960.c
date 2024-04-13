bjc_put_set_compression(gp_file *file, char compression)
{
    bjc_put_command(file, 'b', 1);
    gp_fputc(compression, file);
}