find_match(int lookfor, linenr_T ourscope)
{
    char_u	*look;
    pos_T	*theirscope;
    char_u	*mightbeif;
    int		elselevel;
    int		whilelevel;

    if (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    else
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    while (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = cin_skipcomment(ml_get_curline());
	if (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    // XXX
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum))
	{
	    // if we've gone outside the braces entirely,
	    // we must be out of scope...
	    theirscope = find_start_brace();  // XXX
	    if (theirscope == NULL)
		break;

	    // and if the brace enclosing this is further
	    // back than the one enclosing the else, we're
	    // out of luck too.
	    if (theirscope->lnum < ourscope)
		break;

	    // and if they're enclosed in a *deeper* brace,
	    // then we can ignore it because it's in a
	    // different scope...
	    if (theirscope->lnum > ourscope)
		continue;

	    // if it was an "else" (that's not an "else if")
	    // then we need to go back to another if, so
	    // increment elselevel
	    look = cin_skipcomment(ml_get_curline());
	    if (cin_iselse(look))
	    {
		mightbeif = cin_skipcomment(look + 4);
		if (!cin_isif(mightbeif))
		    ++elselevel;
		continue;
	    }

	    // if it was a "while" then we need to go back to
	    // another "do", so increment whilelevel.  XXX
	    if (cin_iswhileofdo(look, curwin->w_cursor.lnum))
	    {
		++whilelevel;
		continue;
	    }

	    // If it's an "if" decrement elselevel
	    look = cin_skipcomment(ml_get_curline());
	    if (cin_isif(look))
	    {
		elselevel--;
		// When looking for an "if" ignore "while"s that
		// get in the way.
		if (elselevel == 0 && lookfor == LOOKFOR_IF)
		    whilelevel = 0;
	    }

	    // If it's a "do" decrement whilelevel
	    if (cin_isdo(look))
		whilelevel--;

	    // if we've used up all the elses, then
	    // this must be the if that we want!
	    // match the indent level of that if.
	    if (elselevel <= 0 && whilelevel <= 0)
		return OK;
	}
    }
    return FAIL;
}