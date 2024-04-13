passGetRange ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (!(passCode == pass_comma || passCode == pass_rightParen))
    {
      compileError (passNested, "invalid range");
      return 0;
    }
  if (passCode == pass_rightParen)
    {
      passInstructions[passIC++] = 1;
      passInstructions[passIC++] = 1;
      return 1;
    }
  if (!passIsNumber ())
    return 0;
  passInstructions[passIC++] = passHoldNumber;
  passCode = passGetScriptToken ();
  if (!(passCode == pass_comma || passCode == pass_rightParen))
    {
      compileError (passNested, "invalid range");
      return 0;
    }
  if (passCode == pass_rightParen)
    {
      passInstructions[passIC++] = passHoldNumber;
      return 1;
    }
  if (!passIsNumber ())
    return 0;
  passInstructions[passIC++] = passHoldNumber;
  if (!passIsRightParen ())
    return 0;
  return 1;
}