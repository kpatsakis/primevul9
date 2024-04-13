static inline void *actual_base_address(void *const p)
{
  return(*pointer_to_space_for_actual_base_address(p));
}