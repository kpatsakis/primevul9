  StoreContext(DcmQueryRetrieveIndexDatabaseHandle *handle, DIC_US aStatus, const char *fname, DcmFileFormat *ff, OFBool correctUID)
  : dbHandle(handle)
  , status(aStatus)
  , fileName(fname)
  , dcmff(ff)
  , opt_correctUIDPadding(correctUID)
  {}