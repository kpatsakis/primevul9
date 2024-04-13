send_client_notifications (void)
{
  struct {
    pid_t pid;
#ifdef HAVE_W32_SYSTEM
    HANDLE handle;
#else
    int signo;
#endif
  } killed[50];
  int killidx = 0;
  int kidx;
  struct server_local_s *sl;

  for (sl=session_list; sl; sl = sl->next_session)
    {
      if (sl->event_signal && sl->assuan_ctx)
        {
          pid_t pid = assuan_get_pid (sl->assuan_ctx);
#ifdef HAVE_W32_SYSTEM
          HANDLE handle = (void *)sl->event_signal;

          for (kidx=0; kidx < killidx; kidx++)
            if (killed[kidx].pid == pid
                && killed[kidx].handle == handle)
              break;
          if (kidx < killidx)
            log_info ("event %lx (%p) already triggered for client %d\n",
                      sl->event_signal, handle, (int)pid);
          else
            {
              log_info ("triggering event %lx (%p) for client %d\n",
                        sl->event_signal, handle, (int)pid);
              if (!SetEvent (handle))
                log_error ("SetEvent(%lx) failed: %s\n",
                           sl->event_signal, w32_strerror (-1));
              if (killidx < DIM (killed))
                {
                  killed[killidx].pid = pid;
                  killed[killidx].handle = handle;
                  killidx++;
                }
            }
#else /*!HAVE_W32_SYSTEM*/
          int signo = sl->event_signal;

          if (pid != (pid_t)(-1) && pid && signo > 0)
            {
              for (kidx=0; kidx < killidx; kidx++)
                if (killed[kidx].pid == pid
                    && killed[kidx].signo == signo)
                  break;
              if (kidx < killidx)
                log_info ("signal %d already sent to client %d\n",
                          signo, (int)pid);
              else
                {
                  log_info ("sending signal %d to client %d\n",
                            signo, (int)pid);
                  kill (pid, signo);
                  if (killidx < DIM (killed))
                    {
                      killed[killidx].pid = pid;
                      killed[killidx].signo = signo;
                      killidx++;
                    }
                }
            }
#endif /*!HAVE_W32_SYSTEM*/
        }
    }
}