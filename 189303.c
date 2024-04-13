static void redraw_titles(void)
{
    need_maketitle = TRUE;
# ifdef FEAT_WINDOWS
    redraw_tabline = TRUE;
# endif
}