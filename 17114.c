update_curswant(void)
{
    if (curwin->w_set_curswant)
    {
	validate_virtcol();
	curwin->w_curswant = curwin->w_virtcol;
	curwin->w_set_curswant = FALSE;
    }
}