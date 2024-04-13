EmbFile::EmbFile(const Object *efStream)
{
  m_size = -1;
  m_createDate = nullptr;
  m_modDate = nullptr;
  m_checksum = nullptr;
  m_mimetype = nullptr;

  m_objStr = efStream->copy();

  if (efStream->isStream()) {
    // dataDict corresponds to Table 3.41 in the PDF1.6 spec.
    Dict *dataDict = efStream->streamGetDict();

    // subtype is normally the mimetype
    Object subtypeName = dataDict->lookup("Subtype");
    if (subtypeName.isName()) {
      m_mimetype = new GooString(subtypeName.getName());
    }

    // paramDict corresponds to Table 3.42 in the PDF1.6 spec
    Object paramDict = dataDict->lookup("Params");
    if (paramDict.isDict()) {
      Object paramObj = paramDict.dictLookup("ModDate");
      if (paramObj.isString())
        m_modDate = new GooString(paramObj.getString());

      paramObj = paramDict.dictLookup("CreationDate");
      if (paramObj.isString())
        m_createDate = new GooString(paramObj.getString());

      paramObj = paramDict.dictLookup("Size");
      if (paramObj.isInt())
        m_size = paramObj.getInt();

      paramObj = paramDict.dictLookup("CheckSum");
      if (paramObj.isString())
        m_checksum = new GooString(paramObj.getString());
    }
  }
}