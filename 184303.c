select_opt_exact(OnigEncoding enc, OptStr* now, OptStr* alt)
{
  int vn, va;

  vn = now->len;
  va = alt->len;

  if (va == 0) {
    return ;
  }
  else if (vn == 0) {
    copy_opt_exact(now, alt);
    return ;
  }
  else if (vn <= 2 && va <= 2) {
    /* ByteValTable[x] is big value --> low price */
    va = map_position_value(enc, now->s[0]);
    vn = map_position_value(enc, alt->s[0]);

    if (now->len > 1) vn += 5;
    if (alt->len > 1) va += 5;
  }

  vn *= 2;
  va *= 2;

  if (comp_distance_value(&now->mm, &alt->mm, vn, va) > 0)
    copy_opt_exact(now, alt);
}