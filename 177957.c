bjc_put_set_initial(gp_file *file)
{
    bjc_put_bytes(file, (const byte *)"\033[K\002\000\000\017", 7);
}