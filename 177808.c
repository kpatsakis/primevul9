arg_init (struct arg_state *state)
{
  state->ngrn = 0;
  state->nsrn = 0;
  state->nsaa = 0;
#if defined (__APPLE__)
  state->allocating_variadic = 0;
#endif
}