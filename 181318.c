static bool name_is_illegal(const char *name)
{
    return !*name || strchr(name, '/') != NULL;
}