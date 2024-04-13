static inline void **pointer_to_space_for_actual_base_address(void *const p)
{
  return((void **) p-1);
}