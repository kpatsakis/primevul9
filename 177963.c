bjc_put_indexed_image(gp_file *file, int dot_rows, int dot_cols, int layers)
{
    bjc_put_command(file, 'f', 5);
    gp_fputc('R', file); /* per spec */
    gp_fputc(dot_rows, file);
    gp_fputc(dot_cols, file);
    gp_fputc(layers, file);
}