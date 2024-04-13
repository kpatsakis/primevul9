hyphenGetNewState (HyphenDict * dict, HyphenHashTab * hashTab, const
		   CharsString * string)
{
  hyphenHashInsert (hashTab, string, dict->numStates);
  /* predicate is true if dict->numStates is a power of two */
  if (!(dict->numStates & (dict->numStates - 1)))
    dict->states = realloc (dict->states, (dict->numStates << 1) *
			    sizeof (HyphenationState));
  if (!dict->states)
    outOfMemory ();
  dict->states[dict->numStates].hyphenPattern = 0;
  dict->states[dict->numStates].fallbackState = DEFAULTSTATE;
  dict->states[dict->numStates].numTrans = 0;
  dict->states[dict->numStates].trans.pointer = NULL;
  return dict->numStates++;
}