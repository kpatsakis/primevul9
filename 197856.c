passIsNumber ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (passCode != pass_numberFound)
    {
      compileError (passNested, "a number expected");
      return 0;
    }
  return 1;
}