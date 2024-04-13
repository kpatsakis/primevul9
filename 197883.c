passIsRightParen ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (passCode != pass_rightParen)
    {
      compileError (passNested, "')' expected");
      return 0;
    }
  return 1;
}