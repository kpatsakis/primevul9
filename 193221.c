OFCondition WlmActivityManager::NegotiateAssociation( T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function negotiates a presentation context which will be used by this application
//                and the other DICOM appliation that requests an association.
// Parameters   : assoc - [in] The association (network connection to another DICOM application).
// Return Value : OFCondition value denoting success or error.
{
  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  switch( opt_networkTransferSyntax )
  {
    case EXS_LittleEndianImplicit:
      // we only support Little Endian Implicit
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      // we prefer Little Endian Explicit
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      // we prefer Big Endian Explicit
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      // we prefer Deflated Little Endian Explicit
      transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#endif
    default:
      // We prefer explicit transfer syntaxes.
      // If we are running on a Little Endian machine we prefer
      // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
      if (gLocalByteOrder == EBO_LittleEndian)  //defined in dcxfer.h
      {
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      }
      else
      {
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      }
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
  }

  // accept any of the supported abstract syntaxes
  OFCondition cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, (const char**)supportedAbstractSyntaxes, numberOfSupportedAbstractSyntaxes, (const char**)transferSyntaxes, numTransferSyntaxes);

  return cond;
}