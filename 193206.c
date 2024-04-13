void WlmActivityManager::AddProcessToTable( int pid, T_ASC_Association *assoc )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function adds a process to the table that stores process information.
// Parameters   : pid   - [in] the process id of the sub-process which was just started.
//                assoc - [in] The association (network connection to another DICOM application).
// Return Value : none.
{
  WlmProcessSlotType *ps;

  // Allocate some memory for a new item in the list of processes.
  ps = new WlmProcessSlotType ();

  // Remember process information in the new item.
  ASC_getPresentationAddresses( assoc->params, ps->peerName, NULL );
  ASC_getAPTitles( assoc->params, ps->callingAETitle, ps->calledAETitle, NULL );
  ps->processId = pid;
  ps->startTime = time(NULL);
  ps->hasStorageAbility = OFFalse;

  // Add new item to the beginning of the list.
  processTable.push_back(ps);
}