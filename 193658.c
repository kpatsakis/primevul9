distance_value(MinMax* mm)
{
  /* 1000 / (min-max-dist + 1) */
  static const short int dist_vals[] = {
    1000,  500,  333,  250,  200,  167,  143,  125,  111,  100, 
      91,   83,   77,   71,   67,   63,   59,   56,   53,   50, 
      48,   45,   43,   42,   40,   38,   37,   36,   34,   33, 
      32,   31,   30,   29,   29,   28,   27,   26,   26,   25, 
      24,   24,   23,   23,   22,   22,   21,   21,   20,   20, 
      20,   19,   19,   19,   18,   18,   18,   17,   17,   17, 
      16,   16,   16,   16,   15,   15,   15,   15,   14,   14, 
      14,   14,   14,   14,   13,   13,   13,   13,   13,   13, 
      12,   12,   12,   12,   12,   12,   11,   11,   11,   11, 
      11,   11,   11,   11,   11,   10,   10,   10,   10,   10
  };

  OnigLen d;

  if (mm->max == INFINITE_LEN) return 0;

  d = mm->max - mm->min;
  if (d < (OnigLen )(sizeof(dist_vals)/sizeof(dist_vals[0])))
    /* return dist_vals[d] * 16 / (mm->min + 12); */
    return (int )dist_vals[d];
  else
    return 1;
}