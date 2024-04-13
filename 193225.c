void WlmActivityManager::CleanChildren()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : This function takes care of removing items referring to (terminated) subprocess
//                from the table which stores all subprocess information. Three different versions
//                for three different platforms are implemented.
// Parameters   : none.
// Return Value : none.
{

#ifdef HAVE_WAITPID                                           // PLATFORMS THAT HAVE waitpid()
  int options = WNOHANG;
  int stat_loc;
  int child=1;

  while( child > 0 )
  {
    // determine status for child processes
    child = (int)(waitpid(-1, &stat_loc, options));
    if( child == 0 )
    {
      // child not yet finished
    }
    else if( child < 0 )
    {
      if( errno == ECHILD )
      {
        // no children
      }
      else
      {
        DCMWLM_WARN("WlmActivityManager::CleanChildren: Wait for child failed");
      }
    }
    else if( child > 0 )
    {
      // dump some information if required
      DCMWLM_INFO("Cleaned up after child (" << child << ")");

      // remove item from process table
      RemoveProcessFromTable( child );
    }
  }

#elif HAVE_WAIT3                                              // PLATFORMS THAT HAVE wait3()
#if defined(__NeXT__)
  // some systems need a union wait as argument to wait3
  union wait status;
#else
  int status;
#endif
  int options = WNOHANG;
  struct rusage rusage;
  int child = 1;

  while( child > 0 )
  {
    // determine status for child processes
    child = wait3( &status, options, &rusage );
    if( child < 0 )
    {
      if( errno == ECHILD )
      {
        // no children
      }
      else
      {
        DCMWLM_WARN("WlmActivityManager::CleanChildren: Wait for child failed");
      }
    }
    else if( child > 0 )
    {
      // dump some information if required
      DCMWLM_INFO("Cleaned up after child (" << child << ")");

      // remove item from process table
      RemoveProcessFromTable( child );
    }
  }
#else                                                         // OTHER PLATFORMS
// for other platforms without waitpid() and without wait3() we
// don't know how to cleanup after children. Dump an error message.
  DCMWLM_WARN("WlmActivityManager::CleanChildren: Cannot wait for child processes");
#endif
}