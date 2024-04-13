register_commands (assuan_context_t ctx)
{
  static struct {
    const char *name;
    assuan_handler_t handler;
    const char * const help;
  } table[] = {
    { "SERIALNO",     cmd_serialno, hlp_serialno },
    { "LEARN",        cmd_learn,    hlp_learn },
    { "READCERT",     cmd_readcert, hlp_readcert },
    { "READKEY",      cmd_readkey,  hlp_readkey },
    { "SETDATA",      cmd_setdata,  hlp_setdata },
    { "PKSIGN",       cmd_pksign,   hlp_pksign },
    { "PKAUTH",       cmd_pkauth,   hlp_pkauth },
    { "PKDECRYPT",    cmd_pkdecrypt,hlp_pkdecrypt },
    { "INPUT",        NULL },
    { "OUTPUT",       NULL },
    { "GETATTR",      cmd_getattr,  hlp_getattr },
    { "SETATTR",      cmd_setattr,  hlp_setattr },
    { "WRITECERT",    cmd_writecert,hlp_writecert },
    { "WRITEKEY",     cmd_writekey, hlp_writekey },
    { "GENKEY",       cmd_genkey,   hlp_genkey },
    { "RANDOM",       cmd_random,   hlp_random },
    { "PASSWD",       cmd_passwd,   hlp_passwd },
    { "CHECKPIN",     cmd_checkpin, hlp_checkpin },
    { "LOCK",         cmd_lock,     hlp_lock },
    { "UNLOCK",       cmd_unlock,   hlp_unlock },
    { "GETINFO",      cmd_getinfo,  hlp_getinfo },
    { "RESTART",      cmd_restart,  hlp_restart },
    { "DISCONNECT",   cmd_disconnect,hlp_disconnect },
    { "APDU",         cmd_apdu,     hlp_apdu },
    { "KILLSCD",      cmd_killscd,  hlp_killscd },
    { NULL }
  };
  int i, rc;

  for (i=0; table[i].name; i++)
    {
      rc = assuan_register_command (ctx, table[i].name, table[i].handler,
                                    table[i].help);
      if (rc)
        return rc;
    }
  assuan_set_hello_line (ctx, "GNU Privacy Guard's Smartcard server ready");

  assuan_register_reset_notify (ctx, reset_notify);
  assuan_register_option_handler (ctx, option_handler);
  return 0;
}