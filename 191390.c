static inline void *adjust(void *const p)
{
  return(align_to_cache(reserve_space_for_actual_base_address(p)));
}