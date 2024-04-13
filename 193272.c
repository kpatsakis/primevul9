DcmSCP::~DcmSCP()
{
  // If there is an open association, drop it and free memory (just to be sure...)
  if (m_assoc)
  {
    dropAndDestroyAssociation();
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
}