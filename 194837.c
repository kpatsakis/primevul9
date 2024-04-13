hook_exec_end ()
{
    if (hook_exec_recursion > 0)
        hook_exec_recursion--;

    if (hook_exec_recursion == 0)
        hook_remove_deleted ();
}