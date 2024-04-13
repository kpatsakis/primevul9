check_termcode_mouse(
    char_u	*tp,
    int		*slen,
    char_u	*key_name,
    char_u	*modifiers_start,
    int		idx,
    int		*modifiers)
{
    int		j;
    char_u	*p;
# if !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    char_u	bytes[6];
    int		num_bytes;
# endif
    int		mouse_code = 0;	    // init for GCC
    int		is_click, is_drag;
    int		is_release, release_is_ambiguous;
    int		wheel_code = 0;
    int		current_button;
    static int	orig_num_clicks = 1;
    static int	orig_mouse_code = 0x0;
# ifdef CHECK_DOUBLE_CLICK
    static int	orig_mouse_col = 0;
    static int	orig_mouse_row = 0;
    static struct timeval  orig_mouse_time = {0, 0};
    // time of previous mouse click
    struct timeval  mouse_time;		// time of current mouse click
    long	timediff;		// elapsed time in msec
# endif

    is_click = is_drag = is_release = release_is_ambiguous = FALSE;

# if !defined(UNIX) || defined(FEAT_MOUSE_XTERM) || defined(FEAT_GUI) \
    || defined(FEAT_MOUSE_GPM) || defined(FEAT_SYSMOUSE)
    if (key_name[0] == KS_MOUSE
#  ifdef FEAT_MOUSE_GPM
	    || key_name[0] == KS_GPM_MOUSE
#  endif
       )
    {
	/*
	 * For xterm we get "<t_mouse>scr", where s == encoded button state:
	 *	   0x20 = left button down
	 *	   0x21 = middle button down
	 *	   0x22 = right button down
	 *	   0x23 = any button release
	 *	   0x60 = button 4 down (scroll wheel down)
	 *	   0x61 = button 5 down (scroll wheel up)
	 *	add 0x04 for SHIFT
	 *	add 0x08 for ALT
	 *	add 0x10 for CTRL
	 *	add 0x20 for mouse drag (0x40 is drag with left button)
	 *	add 0x40 for mouse move (0x80 is move, 0x81 too)
	 *		 0x43 (drag + release) is also move
	 *  c == column + ' ' + 1 == column + 33
	 *  r == row + ' ' + 1 == row + 33
	 *
	 * The coordinates are passed on through global variables.  Ugly, but
	 * this avoids trouble with mouse clicks at an unexpected moment and
	 * allows for mapping them.
	 */
	for (;;)
	{
#  ifdef FEAT_GUI
	    if (gui.in_use)
	    {
		// GUI uses more bits for columns > 223
		num_bytes = get_bytes_from_buf(tp + *slen, bytes, 5);
		if (num_bytes == -1)	// not enough coordinates
		    return -1;
		mouse_code = bytes[0];
		mouse_col = 128 * (bytes[1] - ' ' - 1)
		    + bytes[2] - ' ' - 1;
		mouse_row = 128 * (bytes[3] - ' ' - 1)
		    + bytes[4] - ' ' - 1;
	    }
	    else
#  endif
	    {
		num_bytes = get_bytes_from_buf(tp + *slen, bytes, 3);
		if (num_bytes == -1)	// not enough coordinates
		    return -1;
		mouse_code = bytes[0];
		mouse_col = bytes[1] - ' ' - 1;
		mouse_row = bytes[2] - ' ' - 1;
	    }
	    *slen += num_bytes;

	    // If the following bytes is also a mouse code and it has the same
	    // code, dump this one and get the next.  This makes dragging a
	    // whole lot faster.
#  ifdef FEAT_GUI
	    if (gui.in_use)
		j = 3;
	    else
#  endif
		j = get_termcode_len(idx);
	    if (STRNCMP(tp, tp + *slen, (size_t)j) == 0
		    && tp[*slen + j] == mouse_code
		    && tp[*slen + j + 1] != NUL
		    && tp[*slen + j + 2] != NUL
#  ifdef FEAT_GUI
		    && (!gui.in_use
			|| (tp[*slen + j + 3] != NUL
			    && tp[*slen + j + 4] != NUL))
#  endif
	       )
		*slen += j;
	    else
		break;
	}
    }

    if (key_name[0] == KS_URXVT_MOUSE
	    || key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	// URXVT 1015 mouse reporting mode:
	// Almost identical to xterm mouse mode, except the values are decimal
	// instead of bytes.
	//
	// \033[%d;%d;%dM
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	//
	// SGR 1006 mouse reporting mode:
	// Almost identical to xterm mouse mode, except the values are decimal
	// instead of bytes.
	//
	// \033[<%d;%d;%dM
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	//
	// \033[<%d;%d;%dm	  : mouse release event
	//	       ^-- row
	//	    ^----- column
	//	 ^-------- code
	p = modifiers_start;
	if (p == NULL)
	    return -1;

	mouse_code = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	// when mouse reporting is SGR, add 32 to mouse code
	if (key_name[0] == KS_SGR_MOUSE
		|| key_name[0] == KS_SGR_MOUSE_RELEASE)
	    mouse_code += 32;

	mouse_col = getdigits(&p) - 1;
	if (*p++ != ';')
	    return -1;

	mouse_row = getdigits(&p) - 1;

	// The modifiers were the mouse coordinates, not the modifier keys
	// (alt/shift/ctrl/meta) state.
	*modifiers = 0;
    }

    if (key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
    {
	if (key_name[0] == KS_SGR_MOUSE_RELEASE)
	{
	    is_release = TRUE;
	    // This is used below to set held_button.
	    mouse_code |= MOUSE_RELEASE;
	}
    }
    else
    {
	release_is_ambiguous = TRUE;
	if ((mouse_code & MOUSE_RELEASE) == MOUSE_RELEASE)
	    is_release = TRUE;
    }

    if (key_name[0] == KS_MOUSE
#  ifdef FEAT_MOUSE_GPM
	    || key_name[0] == KS_GPM_MOUSE
#  endif
#  ifdef FEAT_MOUSE_URXVT
	    || key_name[0] == KS_URXVT_MOUSE
#  endif
	    || key_name[0] == KS_SGR_MOUSE
	    || key_name[0] == KS_SGR_MOUSE_RELEASE)
    {
#  if !defined(MSWIN)
	/*
	 * Handle old style mouse events.
	 * Recognize the xterm mouse wheel, but not in the GUI, the
	 * Linux console with GPM and the MS-DOS or Win32 console
	 * (multi-clicks use >= 0x60).
	 */
	if (mouse_code >= MOUSEWHEEL_LOW
#   ifdef FEAT_GUI
		&& !gui.in_use
#   endif
#   ifdef FEAT_MOUSE_GPM
		&& key_name[0] != KS_GPM_MOUSE
#   endif
	   )
	{
#   if defined(UNIX)
	    if (use_xterm_mouse() > 1 && mouse_code >= 0x80)
		// mouse-move event, using MOUSE_DRAG works
		mouse_code = MOUSE_DRAG;
	    else
#   endif
		// Keep the mouse_code before it's changed, so that we
		// remember that it was a mouse wheel click.
		wheel_code = mouse_code;
	}
#   ifdef FEAT_MOUSE_XTERM
	else if (held_button == MOUSE_RELEASE
#    ifdef FEAT_GUI
		&& !gui.in_use
#    endif
		&& (mouse_code == 0x23 || mouse_code == 0x24
		    || mouse_code == 0x40 || mouse_code == 0x41))
	{
	    // Apparently 0x23 and 0x24 are used by rxvt scroll wheel.
	    // And 0x40 and 0x41 are used by some xterm emulator.
	    wheel_code = mouse_code - (mouse_code >= 0x40 ? 0x40 : 0x23)
							      + MOUSEWHEEL_LOW;
	}
#   endif

#   if defined(UNIX)
	else if (use_xterm_mouse() > 1)
	{
	    if (mouse_code & MOUSE_DRAG_XTERM)
		mouse_code |= MOUSE_DRAG;
	}
#   endif
#   ifdef FEAT_XCLIPBOARD
	else if (!(mouse_code & MOUSE_DRAG & ~MOUSE_CLICK_MASK))
	{
	    if (is_release)
		stop_xterm_trace();
	    else
		start_xterm_trace(mouse_code);
	}
#   endif
#  endif
    }
# endif // !UNIX || FEAT_MOUSE_XTERM
# ifdef FEAT_MOUSE_NET
    if (key_name[0] == KS_NETTERM_MOUSE)
    {
	int mc, mr;

	// expect a rather limited sequence like: balancing {
	// \033}6,45\r
	// '6' is the row, 45 is the column
	p = tp + *slen;
	mr = getdigits(&p);
	if (*p++ != ',')
	    return -1;
	mc = getdigits(&p);
	if (*p++ != '\r')
	    return -1;

	mouse_col = mc - 1;
	mouse_row = mr - 1;
	mouse_code = MOUSE_LEFT;
	*slen += (int)(p - (tp + *slen));
    }
# endif	// FEAT_MOUSE_NET
# ifdef FEAT_MOUSE_JSB
    if (key_name[0] == KS_JSBTERM_MOUSE)
    {
	int mult, val, iter, button, status;

	/*
	 * JSBTERM Input Model
	 * \033[0~zw uniq escape sequence
	 * (L-x)  Left button pressed - not pressed x not reporting
	 * (M-x)  Middle button pressed - not pressed x not reporting
	 * (R-x)  Right button pressed - not pressed x not reporting
	 * (SDmdu)  Single , Double click, m: mouse move, d: button down,
		      *						   u: button up
	 *  ###   X cursor position padded to 3 digits
	 *  ###   Y cursor position padded to 3 digits
	 * (s-x)  SHIFT key pressed - not pressed x not reporting
	 * (c-x)  CTRL key pressed - not pressed x not reporting
	 * \033\\ terminating sequence
	 */
	p = tp + *slen;
	button = mouse_code = 0;
	switch (*p++)
	{
	    case 'L': button = 1; break;
	    case '-': break;
	    case 'x': break; // ignore sequence
	    default:  return -1; // Unknown Result
	}
	switch (*p++)
	{
	    case 'M': button |= 2; break;
	    case '-': break;
	    case 'x': break; // ignore sequence
	    default:  return -1; // Unknown Result
	}
	switch (*p++)
	{
	    case 'R': button |= 4; break;
	    case '-': break;
	    case 'x': break; // ignore sequence
	    default:  return -1; // Unknown Result
	}
	status = *p++;
	for (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    if (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    else
		return -1;
	mouse_col = val;
	for (val = 0, mult = 100, iter = 0; iter < 3; iter++,
		mult /= 10, p++)
	    if (*p >= '0' && *p <= '9')
		val += (*p - '0') * mult;
	    else
		return -1;
	mouse_row = val;
	switch (*p++)
	{
	    case 's': button |= 8; break;  // SHIFT key Pressed
	    case '-': break;  // Not Pressed
	    case 'x': break;  // Not Reporting
	    default:  return -1; // Unknown Result
	}
	switch (*p++)
	{
	    case 'c': button |= 16; break;  // CTRL key Pressed
	    case '-': break;  // Not Pressed
	    case 'x': break;  // Not Reporting
	    default:  return -1; // Unknown Result
	}
	if (*p++ != '\033')
	    return -1;
	if (*p++ != '\\')
	    return -1;
	switch (status)
	{
	    case 'D': // Double Click
	    case 'S': // Single Click
		if (button & 1) mouse_code |= MOUSE_LEFT;
		if (button & 2) mouse_code |= MOUSE_MIDDLE;
		if (button & 4) mouse_code |= MOUSE_RIGHT;
		if (button & 8) mouse_code |= MOUSE_SHIFT;
		if (button & 16) mouse_code |= MOUSE_CTRL;
		break;
	    case 'm': // Mouse move
		if (button & 1) mouse_code |= MOUSE_LEFT;
		if (button & 2) mouse_code |= MOUSE_MIDDLE;
		if (button & 4) mouse_code |= MOUSE_RIGHT;
		if (button & 8) mouse_code |= MOUSE_SHIFT;
		if (button & 16) mouse_code |= MOUSE_CTRL;
		if ((button & 7) != 0)
		{
		    held_button = mouse_code;
		    mouse_code |= MOUSE_DRAG;
		}
		is_drag = TRUE;
		showmode();
		break;
	    case 'd': // Button Down
		if (button & 1) mouse_code |= MOUSE_LEFT;
		if (button & 2) mouse_code |= MOUSE_MIDDLE;
		if (button & 4) mouse_code |= MOUSE_RIGHT;
		if (button & 8) mouse_code |= MOUSE_SHIFT;
		if (button & 16) mouse_code |= MOUSE_CTRL;
		break;
	    case 'u': // Button Up
		is_release = TRUE;
		if (button & 1)
		    mouse_code |= MOUSE_LEFT;
		if (button & 2)
		    mouse_code |= MOUSE_MIDDLE;
		if (button & 4)
		    mouse_code |= MOUSE_RIGHT;
		if (button & 8)
		    mouse_code |= MOUSE_SHIFT;
		if (button & 16)
		    mouse_code |= MOUSE_CTRL;
		break;
	    default: return -1; // Unknown Result
	}

	*slen += (p - (tp + *slen));
    }
# endif // FEAT_MOUSE_JSB
# ifdef FEAT_MOUSE_DEC
    if (key_name[0] == KS_DEC_MOUSE)
    {
	/*
	 * The DEC Locator Input Model
	 * Netterm delivers the code sequence:
	 *  \033[2;4;24;80&w  (left button down)
	 *  \033[3;0;24;80&w  (left button up)
	 *  \033[6;1;24;80&w  (right button down)
	 *  \033[7;0;24;80&w  (right button up)
	 * CSI Pe ; Pb ; Pr ; Pc ; Pp & w
	 * Pe is the event code
	 * Pb is the button code
	 * Pr is the row coordinate
	 * Pc is the column coordinate
	 * Pp is the third coordinate (page number)
	 * Pe, the event code indicates what event caused this report
	 *    The following event codes are defined:
	 *    0 - request, the terminal received an explicit request for a
	 *	  locator report, but the locator is unavailable
	 *    1 - request, the terminal received an explicit request for a
	 *	  locator report
	 *    2 - left button down
	 *    3 - left button up
	 *    4 - middle button down
	 *    5 - middle button up
	 *    6 - right button down
	 *    7 - right button up
	 *    8 - fourth button down
	 *    9 - fourth button up
	 *    10 - locator outside filter rectangle
	 * Pb, the button code, ASCII decimal 0-15 indicating which buttons are
	 *   down if any. The state of the four buttons on the locator
	 *   correspond to the low four bits of the decimal value, "1" means
	 *   button depressed
	 *   0 - no buttons down,
	 *   1 - right,
	 *   2 - middle,
	 *   4 - left,
	 *   8 - fourth
	 * Pr is the row coordinate of the locator position in the page,
	 *   encoded as an ASCII decimal value.  If Pr is omitted, the locator
	 *   position is undefined (outside the terminal window for example).
	 * Pc is the column coordinate of the locator position in the page,
	 *   encoded as an ASCII decimal value.  If Pc is omitted, the locator
	 *   position is undefined (outside the terminal window for example).
	 * Pp is the page coordinate of the locator position encoded as an
	 *   ASCII decimal value.  The page coordinate may be omitted if the
	 *   locator is on page one (the default).  We ignore it anyway.
	 */
	int Pe, Pb, Pr, Pc;

	p = tp + *slen;

	// get event status
	Pe = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	// get button status
	Pb = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	// get row status
	Pr = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	// get column status
	Pc = getdigits(&p);

	// the page parameter is optional
	if (*p == ';')
	{
	    p++;
	    (void)getdigits(&p);
	}
	if (*p++ != '&')
	    return -1;
	if (*p++ != 'w')
	    return -1;

	mouse_code = 0;
	switch (Pe)
	{
	    case  0: return -1; // position request while unavailable
	    case  1: // a response to a locator position request includes
		     //	the status of all buttons
		     Pb &= 7;   // mask off and ignore fourth button
		     if (Pb & 4)
			 mouse_code  = MOUSE_LEFT;
		     if (Pb & 2)
			 mouse_code  = MOUSE_MIDDLE;
		     if (Pb & 1)
			 mouse_code  = MOUSE_RIGHT;
		     if (Pb)
		     {
			 held_button = mouse_code;
			 mouse_code |= MOUSE_DRAG;
			 WantQueryMouse = TRUE;
		     }
		     is_drag = TRUE;
		     showmode();
		     break;
	    case  2: mouse_code = MOUSE_LEFT;
		     WantQueryMouse = TRUE;
		     break;
	    case  3: mouse_code = MOUSE_LEFT;
		     is_release = TRUE;
		     break;
	    case  4: mouse_code = MOUSE_MIDDLE;
		     WantQueryMouse = TRUE;
		     break;
	    case  5: mouse_code = MOUSE_MIDDLE;
		     is_release = TRUE;
		     break;
	    case  6: mouse_code = MOUSE_RIGHT;
		     WantQueryMouse = TRUE;
		     break;
	    case  7: mouse_code = MOUSE_RIGHT;
		     is_release = TRUE;
		     break;
	    case  8: return -1; // fourth button down
	    case  9: return -1; // fourth button up
	    case 10: return -1; // mouse outside of filter rectangle
	    default: return -1; // should never occur
	}

	mouse_col = Pc - 1;
	mouse_row = Pr - 1;

	*slen += (int)(p - (tp + *slen));
    }
# endif // FEAT_MOUSE_DEC
# ifdef FEAT_MOUSE_PTERM
    if (key_name[0] == KS_PTERM_MOUSE)
    {
	int button, num_clicks, action;

	p = tp + *slen;

	action = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	mouse_row = getdigits(&p);
	if (*p++ != ';')
	    return -1;
	mouse_col = getdigits(&p);
	if (*p++ != ';')
	    return -1;

	button = getdigits(&p);
	mouse_code = 0;

	switch (button)
	{
	    case 4: mouse_code = MOUSE_LEFT; break;
	    case 1: mouse_code = MOUSE_RIGHT; break;
	    case 2: mouse_code = MOUSE_MIDDLE; break;
	    default: return -1;
	}

	switch (action)
	{
	    case 31: // Initial press
		if (*p++ != ';')
		    return -1;

		num_clicks = getdigits(&p); // Not used
		break;

	    case 32: // Release
		is_release = TRUE;
		break;

	    case 33: // Drag
		held_button = mouse_code;
		mouse_code |= MOUSE_DRAG;
		break;

	    default:
		return -1;
	}

	if (*p++ != 't')
	    return -1;

	*slen += (p - (tp + *slen));
    }
# endif // FEAT_MOUSE_PTERM

    // Interpret the mouse code
    current_button = (mouse_code & MOUSE_CLICK_MASK);
    if (is_release)
	current_button |= MOUSE_RELEASE;

    if (current_button == MOUSE_RELEASE
# ifdef FEAT_MOUSE_XTERM
	    && wheel_code == 0
# endif
       )
    {
	/*
	 * If we get a mouse drag or release event when there is no mouse
	 * button held down (held_button == MOUSE_RELEASE), produce a K_IGNORE
	 * below.
	 * (can happen when you hold down two buttons and then let them go, or
	 * click in the menu bar, but not on a menu, and drag into the text).
	 */
	if ((mouse_code & MOUSE_DRAG) == MOUSE_DRAG)
	    is_drag = TRUE;
	current_button = held_button;
    }
    else
    {
      if (wheel_code == 0)
      {
# ifdef CHECK_DOUBLE_CLICK
#  ifdef FEAT_MOUSE_GPM
	/*
	 * Only for Unix, when GUI not active, we handle multi-clicks here, but
	 * not for GPM mouse events.
	 */
#   ifdef FEAT_GUI
	if (key_name[0] != KS_GPM_MOUSE && !gui.in_use)
#   else
	    if (key_name[0] != KS_GPM_MOUSE)
#   endif
#  else
#   ifdef FEAT_GUI
		if (!gui.in_use)
#   endif
#  endif
		{
		    /*
		     * Compute the time elapsed since the previous mouse click.
		     */
		    gettimeofday(&mouse_time, NULL);
		    if (orig_mouse_time.tv_sec == 0)
		    {
			/*
			 * Avoid computing the difference between mouse_time
			 * and orig_mouse_time for the first click, as the
			 * difference would be huge and would cause
			 * multiplication overflow.
			 */
			timediff = p_mouset;
		    }
		    else
			timediff = time_diff_ms(&orig_mouse_time, &mouse_time);
		    orig_mouse_time = mouse_time;
		    if (mouse_code == orig_mouse_code
			    && timediff < p_mouset
			    && orig_num_clicks != 4
			    && orig_mouse_col == mouse_col
			    && orig_mouse_row == mouse_row
			    && (is_mouse_topline(curwin)
				// Double click in tab pages line also works
				// when window contents changes.
				|| (mouse_row == 0 && firstwin->w_winrow > 0))
		       )
			++orig_num_clicks;
		    else
			orig_num_clicks = 1;
		    orig_mouse_col = mouse_col;
		    orig_mouse_row = mouse_row;
		    set_mouse_topline(curwin);
		}
#  if defined(FEAT_GUI) || defined(FEAT_MOUSE_GPM)
		else
		    orig_num_clicks = NUM_MOUSE_CLICKS(mouse_code);
#  endif
# else
	orig_num_clicks = NUM_MOUSE_CLICKS(mouse_code);
# endif
	is_click = TRUE;
      }
      orig_mouse_code = mouse_code;
    }
    if (!is_drag)
	held_button = mouse_code & MOUSE_CLICK_MASK;

    /*
     * Translate the actual mouse event into a pseudo mouse event.
     * First work out what modifiers are to be used.
     */
    if (orig_mouse_code & MOUSE_SHIFT)
	*modifiers |= MOD_MASK_SHIFT;
    if (orig_mouse_code & MOUSE_CTRL)
	*modifiers |= MOD_MASK_CTRL;
    if (orig_mouse_code & MOUSE_ALT)
	*modifiers |= MOD_MASK_ALT;
    if (orig_num_clicks == 2)
	*modifiers |= MOD_MASK_2CLICK;
    else if (orig_num_clicks == 3)
	*modifiers |= MOD_MASK_3CLICK;
    else if (orig_num_clicks == 4)
	*modifiers |= MOD_MASK_4CLICK;

    // Work out our pseudo mouse event. Note that MOUSE_RELEASE gets added,
    // then it's not mouse up/down.
    key_name[0] = KS_EXTRA;
    if (wheel_code != 0 && (!is_release || release_is_ambiguous))
    {
	if (wheel_code & MOUSE_CTRL)
	    *modifiers |= MOD_MASK_CTRL;
	if (wheel_code & MOUSE_ALT)
	    *modifiers |= MOD_MASK_ALT;

	if (wheel_code & 1 && wheel_code & 2)
	    key_name[1] = (int)KE_MOUSELEFT;
	else if (wheel_code & 2)
	    key_name[1] = (int)KE_MOUSERIGHT;
	else if (wheel_code & 1)
	    key_name[1] = (int)KE_MOUSEUP;
	else
	    key_name[1] = (int)KE_MOUSEDOWN;

	held_button = MOUSE_RELEASE;
    }
    else
	key_name[1] = get_pseudo_mouse_code(current_button, is_click, is_drag);


    // Make sure the mouse position is valid.  Some terminals may return weird
    // values.
    if (mouse_col >= Columns)
	mouse_col = Columns - 1;
    if (mouse_row >= Rows)
	mouse_row = Rows - 1;

    return 0;
}