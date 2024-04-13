void DcmSCP::notifyDIMSEError(const OFCondition &cond)
{
  OFString tempStr;
  DCMNET_DEBUG("DIMSE Error, detail (if available): " << DimseCondition::dump(tempStr, cond));
}