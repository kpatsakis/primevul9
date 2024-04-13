text_locked(void)
{
    return text_and_win_locked() || textlock != 0;
}