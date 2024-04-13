OFBool DcmSCP::addStatusDetail(DcmDataset **statusDetail,
                               const DcmElement *elem)
{
  DCMNET_TRACE("Add element to status detail");
  // If no element was passed, return to the caller.
  if( elem == NULL )
    return OFFalse;

  DcmAttributeTag *at;
  DcmLongString *lo;

  // Create the container object if necessary
  if( *statusDetail == NULL )
    *statusDetail = new DcmDataset();

  if (statusDetail == NULL)
  {
    DCMNET_ERROR("Cannot create status detail object, memory exhausted!");
    return OFFalse;
  }

  // Determine the element's data type
  DcmVR vr( elem->ident() );

  // Depending on the element's identification, insert different
  // types of objects into the container.
  switch( elem->ident() )
  {
    case EVR_LO:
      lo = new DcmLongString( *((DcmLongString*)elem) );
      if( lo->getLength() > vr.getMaxValueLength() )
      {
        DCMNET_WARN("Value inside given LO attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
      }
      (*statusDetail)->insert( lo, OFTrue /*replaceOld*/ );
      // Print debug information
      {
        OFOStringStream oss;
        lo->print(oss);
        OFSTRINGSTREAM_GETSTR(oss, strtemp);
        DCMNET_DEBUG("Adding LO status detail information: " << strtemp);
        OFSTRINGSTREAM_FREESTR(tmpString)
      }
      break;
    case EVR_AT:
      at = new DcmAttributeTag( *((DcmAttributeTag*)elem) );
      if( at->getLength() > vr.getMaxValueLength() )
      {
        DCMNET_WARN("Value inside given AT attribute too large for status detail (max " << OFstatic_cast(Uint32, vr.getMaxValueLength()) << ") for " << vr.getVRName());
      }
      (*statusDetail)->insert( at, OFTrue /*replaceOld*/ );
      // Print debug information
      {
        OFOStringStream oss;
        at->print(oss);
        OFSTRINGSTREAM_GETSTR(oss, strtemp);
        DCMNET_DEBUG("Adding AT status detail information: " << strtemp);
        OFSTRINGSTREAM_FREESTR(tmpString)
      }
      break;
    default:  // other status detail is not supported
      DCMNET_ERROR("Cannot add status detail, unsupported detail attribute type: " << vr.getVRName());
      return OFFalse;
      break;
  }
  return OFTrue;
}