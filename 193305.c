WlmActivityManager::~WlmActivityManager()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  delete[] supportedAbstractSyntaxes[0];
  delete[] supportedAbstractSyntaxes[1];
  delete[] supportedAbstractSyntaxes;

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
}