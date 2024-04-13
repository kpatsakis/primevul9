bjc_put_media_supply(gp_file *file, char supply, char type)
{
    bjc_put_command(file, 'l', 2);
    gp_fputc(supply, file);
    gp_fputc(type << 4, file);
}