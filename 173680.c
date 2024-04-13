static struct SYMBOL *next_lyric_note(struct SYMBOL *s)
{
	while (s
	    && (s->abc_type != ABC_T_NOTE
	     || (s->flags & ABC_F_GRACE)))
		s = s->next;
	return s;
}