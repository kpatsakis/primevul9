static OFString AddStatusDetail( DcmDataset **statusDetail, const DcmElement *elem )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function adds information to the status detail information container.
// Parameters   : statusDetail - [inout] This variable can be used to capture detailed information
//                               with regard to the status information which is captured in the
//                               status element (0000,0900) of the C-FIND-RSP message.
//                elem         - [in] Element that shall be added to the status detail information
//                               container.
// Return Value : none.
{
  // If no element was passed, return to the caller.
  if( elem == NULL )
    return "";

  OFOStringStream log;
  DcmAttributeTag *at;
  DcmLongString *lo;

  // Create the container object if necessary
  if( *statusDetail == NULL )
    *statusDetail = new DcmDataset();

  // Determine the element's data type
  DcmVR vr( elem->ident() );

  // Dump some information
  log << "Status Detail:" << OFendl;

  // Depending on the element's identification, insert different
  // types of objects into the container.
  switch( elem->ident() )
  {
    case EVR_LO:
      lo = new DcmLongString( *((DcmLongString*)elem) );
      if( lo->getLength() > vr.getMaxValueLength() )
      {
        log << "AddStatusDetail: INTERNAL ERROR: value too large (max " << (unsigned long)(vr.getMaxValueLength())
            << ") for " << vr.getVRName() << ": " << OFendl;
      }
      (*statusDetail)->insert( lo, OFTrue /*replaceOld*/ );
      lo->print(log);
      break;
    case EVR_AT:
      at = new DcmAttributeTag( *((DcmAttributeTag*)elem) );
      if( at->getLength() > vr.getMaxValueLength() )
      {
        log << "AddStatusDetail: INTERNAL ERROR: value too large (max " << (unsigned long)(vr.getMaxValueLength())
            << ") for " << vr.getVRName() << ": " << OFendl;
      }
      (*statusDetail)->insert( at, OFTrue /*replaceOld*/ );
      at->print(log);
      break;
    default:
      // other status detail is not supported
      log << "AddStatusDetail: unsupported status detail type: " << vr.getVRName() << OFendl;
      break;
  }
  log << OFStringStream_ends;
  OFSTRINGSTREAM_GETOFSTRING(log, ret)
  return ret;
}