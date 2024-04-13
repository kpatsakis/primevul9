static int errorCond(OFCondition cond, const char *message)
{
  int result = (cond.bad());
  if (result)
  {
    OFString temp_str;
    OFLOG_ERROR(dcmpsrcvLogger, message << OFendl << DimseCondition::dump(temp_str, cond));
  }
  return result;
}