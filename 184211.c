distance_add(OnigLen d1, OnigLen d2)
{
  if (d1 == INFINITE_LEN || d2 == INFINITE_LEN)
    return INFINITE_LEN;
  else {
    if (d1 <= INFINITE_LEN - d2) return d1 + d2;
    else return INFINITE_LEN;
  }
}