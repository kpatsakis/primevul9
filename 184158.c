map_position_value(OnigEncoding enc, int i)
{
  static const short int Vals[] = {
     5,  1,  1,  1,  1,  1,  1,  1,  1, 10, 10,  1,  1, 10,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    12,  4,  7,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,
     5,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  6,  5,  5,  5,
     5,  6,  6,  6,  6,  7,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  1
  };

  if (i < (int )(sizeof(Vals)/sizeof(Vals[0]))) {
    if (i == 0 && ONIGENC_MBC_MINLEN(enc) > 1)
      return 20;
    else
      return (int )Vals[i];
  }
  else
    return 4;   /* Take it easy. */
}