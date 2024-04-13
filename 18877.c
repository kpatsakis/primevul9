out_flush_cursor(
    int	    force UNUSED,   // when TRUE, update cursor even when not moved
    int	    clear_selection UNUSED) // clear selection under cursor
{
    mch_disable_flush();
    out_flush();
    mch_enable_flush();
#ifdef FEAT_GUI
    if (gui.in_use)
    {
	gui_update_cursor(force, clear_selection);
	gui_may_flush();
    }
#endif
}