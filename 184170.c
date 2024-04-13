add_char_opt_map(OptMap* m, UChar c, OnigEncoding enc)
{
  if (m->map[c] == 0) {
    m->map[c] = 1;
    m->value += map_position_value(enc, c);
  }
}