passIsLeftParen ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (passCode != pass_leftParen)
    {
      compileError (passNested, "'(' expected");
      return 0;
    }
  return 1;
}