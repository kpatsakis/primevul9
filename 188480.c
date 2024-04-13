void ga_command_state_init(GAState *s, GACommandState *cs)
{
#if defined(CONFIG_FSFREEZE)
    ga_command_state_add(cs, NULL, guest_fsfreeze_cleanup);
#endif
}