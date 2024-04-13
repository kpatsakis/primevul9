hyphenAddTrans (HyphenDict * dict, int state1, int state2, widechar ch)
{
  int numTrans;
  numTrans = dict->states[state1].numTrans;
  if (numTrans == 0)
    dict->states[state1].trans.pointer = malloc (sizeof (HyphenationTrans));
  else if (!(numTrans & (numTrans - 1)))
    dict->states[state1].trans.pointer = realloc
      (dict->states[state1].trans.pointer,
       (numTrans << 1) * sizeof (HyphenationTrans));
  dict->states[state1].trans.pointer[numTrans].ch = ch;
  dict->states[state1].trans.pointer[numTrans].newState = state2;
  dict->states[state1].numTrans++;
}