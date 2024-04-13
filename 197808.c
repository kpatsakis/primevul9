passIsComma ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (passCode != pass_comma)
    {
      compileError (passNested, "',' expected");
      return 0;
    }
  return 1;
}