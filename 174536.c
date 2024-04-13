delivery_re_exec(int exec_type)
{
uschar * where;

if (cutthrough.fd >= 0 && cutthrough.callout_hold_only)
  {
  int pfd[2], channel_fd = cutthrough.fd, pid;

  smtp_peer_options = cutthrough.peer_options;
  continue_sequence = 0;

#ifdef SUPPORT_TLS
  if (cutthrough.is_tls)
    {
    smtp_peer_options |= OPTION_TLS;
    sending_ip_address = cutthrough.snd_ip;
    sending_port = cutthrough.snd_port;

    where = US"socketpair";
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pfd) != 0)
      goto fail;

    where = US"fork";
    if ((pid = fork()) < 0)
      goto fail;

    else if (pid == 0)		/* child: fork again to totally disconnect */
      {
      if (running_in_test_harness) millisleep(100); /* let parent debug out */
      /* does not return */
      smtp_proxy_tls(big_buffer, big_buffer_size, pfd, 5*60);
      }

    DEBUG(D_transport) debug_printf("proxy-proc inter-pid %d\n", pid);
    close(pfd[0]);
    waitpid(pid, NULL, 0);
    (void) close(channel_fd);	/* release the client socket */
    channel_fd = pfd[1];
    }
#endif

  transport_do_pass_socket(cutthrough.transport, cutthrough.host.name,
    cutthrough.host.address, message_id, channel_fd);
  }
else
  {
  cancel_cutthrough_connection(TRUE, US"non-continued delivery");
  (void) child_exec_exim(exec_type, FALSE, NULL, FALSE, 2, US"-Mc", message_id);
  }
return;		/* compiler quietening; control does not reach here. */

fail:
  log_write(0,
    LOG_MAIN | (exec_type == CEE_EXEC_EXIT ? LOG_PANIC : LOG_PANIC_DIE),
    "delivery re-exec %s failed: %s", where, strerror(errno));

  /* Get here if exec_type == CEE_EXEC_EXIT.
  Note: this must be _exit(), not exit(). */

  _exit(EX_EXECFAILED);
}