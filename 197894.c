passIsName ()
{
  pass_Codes passCode = passGetScriptToken ();
  if (passCode != pass_nameFound)
    {
      compileError (passNested, "a name expected");
      return 0;
    }
  return 1;
}