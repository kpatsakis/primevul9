comp_opt_exact_or_map(OptStr* e, OptMap* m)
{
#define COMP_EM_BASE  20
  int ae, am;
  int case_value;

  if (m->value <= 0) return -1;

  case_value = 3;
  ae = COMP_EM_BASE * e->len * case_value;
  am = COMP_EM_BASE * 5 * 2 / m->value;
  return comp_distance_value(&e->mm, &m->mm, ae, am);
}