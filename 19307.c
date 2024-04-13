option_handler (assuan_context_t ctx, const char *key, const char *value)
{
  ctrl_t ctrl = assuan_get_pointer (ctx);

  if (!strcmp (key, "event-signal"))
    {
      /* A value of 0 is allowed to reset the event signal. */
#ifdef HAVE_W32_SYSTEM
      if (!*value)
        return gpg_error (GPG_ERR_ASS_PARAMETER);
      ctrl->server_local->event_signal = strtoul (value, NULL, 16);
#else
      int i = *value? atoi (value) : -1;
      if (i < 0)
        return gpg_error (GPG_ERR_ASS_PARAMETER);
      ctrl->server_local->event_signal = i;
#endif
    }

 return 0;
}