add_opcode_option(regex_t* reg, int opcode, OnigOptionType option)
{
  int r;

  r = add_opcode(reg, opcode);
  if (r != 0) return r;
  r = add_option(reg, option);
  return r;
}