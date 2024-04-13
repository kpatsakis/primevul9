void WlmActivityManager::RemoveProcessFromTable( int pid )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function removes one particular item from the table which stores all subprocess
//                information. The item which shall be deleted will be identified by its process id.
// Parameters   : pid - [in] process id.
// Return Value : none.
{
  WlmProcessSlotType *ps = NULL;

  // try to find item that corresponds to the given process id
  OFListIterator(WlmProcessSlotType*) it = processTable.begin();
  while ( it != processTable.end() )
  {
    ps = *it;
    // if process can be found, delete it from list and free memory
    if ( ps->processId == pid )
    {
      processTable.remove(*it);
      delete ps;
      return;
    }
    it++;
  }

  // dump a warning if process could not be found in process table
  DCMWLM_WARN("WlmActivityManager::RemoveProcessFromTable: Could not find process " << pid);
}