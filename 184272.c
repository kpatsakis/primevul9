print_distance_range(FILE* f, OnigLen a, OnigLen b)
{
  if (a == INFINITE_LEN)
    fputs("inf", f);
  else
    fprintf(f, "(%u)", a);

  fputs("-", f);

  if (b == INFINITE_LEN)
    fputs("inf", f);
  else
    fprintf(f, "(%u)", b);
}