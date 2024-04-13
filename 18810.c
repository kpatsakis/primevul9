term_is_8bit(char_u *name)
{
    return (detected_8bit || strstr((char *)name, "8bit") != NULL);
}