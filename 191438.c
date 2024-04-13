static inline void *reserve_space_for_actual_base_address(void *const p)
{
  return((void **) p+1);
}