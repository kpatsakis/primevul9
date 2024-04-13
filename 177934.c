bjc_put_page_id(gp_file *file, int id)
{
    bjc_put_command(file, 'q', 1);
    gp_fputc(id, file);
}