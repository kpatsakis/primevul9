cin_starts_with(char_u *s, char *word)
{
    int l = (int)STRLEN(word);

    return (STRNCMP(s, word, l) == 0 && !vim_isIDc(s[l]));
}