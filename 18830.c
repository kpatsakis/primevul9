term_is_builtin(char_u *name)
{
    return (STRNCMP(name, "builtin_", (size_t)8) == 0);
}