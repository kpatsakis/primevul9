op2name(int opcode)
{
  int i;

  for (i = 0; OpInfo[i].opcode >= 0; i++) {
    if (opcode == OpInfo[i].opcode)
      return OpInfo[i].name;
  }
  return "";
}