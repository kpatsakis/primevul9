term_is_gui(char_u *name)
{
    return (STRCMP(name, "builtin_gui") == 0 || STRCMP(name, "gui") == 0);
}