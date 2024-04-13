allocate_int_to_reg_or_stack (struct call_context *context,
			      struct arg_state *state,
			      void *stack, size_t size)
{
  if (state->ngrn < N_X_ARG_REG)
    return &context->x[state->ngrn++];

  state->ngrn = N_X_ARG_REG;
  return allocate_to_stack (state, stack, size, size);
}