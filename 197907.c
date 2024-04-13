passInsertAttributes ()
{
  passInstructions[passIC++] = pass_attributes;
  passInstructions[passIC++] = passAttributes >> 16;
  passInstructions[passIC++] = passAttributes & 0xffff;
  if (!passGetRange ())
    return 0;
  return 1;
}