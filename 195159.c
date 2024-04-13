EmbFile::~EmbFile()
{
  delete m_createDate;
  delete m_modDate;
  delete m_checksum;
  delete m_mimetype;
}