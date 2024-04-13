Object Parser::getObj(int recursion)
{
  return getObj(gFalse, nullptr, cryptRC4, 0, 0, 0, recursion);
}