static void executeEndOfStudyEvents()
    /*
     * This function deals with the execution of end-of-study-events. In detail,
     * events that need to take place are specified by the user through certain
     * command line options. The options that define these end-of-study-events
     * are "--rename-on-eostudy" and "--exec-on-eostudy".
     *
     * Parameters:
     *   none.
     */
{
  // if option --rename-on-eostudy is set and variable lastStudySubdirectoryPathAndName
  // does not equal NULL (i.e. we received all objects that belong to one study, or - in
  // other words - it is the end of one study) we want to rename the output files that
  // belong to the last study. (Note that these files are captured in outputFileNameArray)
  if( opt_renameOnEndOfStudy && !lastStudySubdirectoryPathAndName.empty() )
    renameOnEndOfStudy();

  // if option --exec-on-eostudy is set and variable lastStudySubdirectoryPathAndName does
  // not equal NULL (i.e. we received all objects that belong to one study, or - in other
  // words - it is the end of one study) we want to execute a certain command which was
  // passed to the application
  if( opt_execOnEndOfStudy != NULL && !lastStudySubdirectoryPathAndName.empty() )
    executeOnEndOfStudy();

  lastStudySubdirectoryPathAndName.clear();
}