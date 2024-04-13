save_parse_state(parse_state_T *ps)
{
    ps->regparse = regparse;
    ps->prevchr_len = prevchr_len;
    ps->curchr = curchr;
    ps->prevchr = prevchr;
    ps->prevprevchr = prevprevchr;
    ps->nextchr = nextchr;
    ps->at_start = at_start;
    ps->prev_at_start = prev_at_start;
    ps->regnpar = regnpar;
}