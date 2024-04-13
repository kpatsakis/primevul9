is_mouse_key(int c)
{
    return c == K_LEFTMOUSE
	|| c == K_LEFTMOUSE_NM
	|| c == K_LEFTDRAG
	|| c == K_LEFTRELEASE
	|| c == K_LEFTRELEASE_NM
	|| c == K_MOUSEMOVE
	|| c == K_MIDDLEMOUSE
	|| c == K_MIDDLEDRAG
	|| c == K_MIDDLERELEASE
	|| c == K_RIGHTMOUSE
	|| c == K_RIGHTDRAG
	|| c == K_RIGHTRELEASE
	|| c == K_MOUSEDOWN
	|| c == K_MOUSEUP
	|| c == K_MOUSELEFT
	|| c == K_MOUSERIGHT
	|| c == K_X1MOUSE
	|| c == K_X1DRAG
	|| c == K_X1RELEASE
	|| c == K_X2MOUSE
	|| c == K_X2DRAG
	|| c == K_X2RELEASE;
}