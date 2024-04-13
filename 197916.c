passGetScriptToken ()
{
  while (passLinepos < passLine.length)
    {
      passPrevLinepos = passLinepos;
      switch (passLine.chars[passLinepos])
	{
	case '\"':
	  passLinepos++;
	  if (passGetString ())
	    return pass_string;
	  return pass_invalidToken;
	case '@':
	  passLinepos++;
	  if (passGetDots ())
	    return pass_dots;
	  return pass_invalidToken;
	case '#':		/*comment */
	  passLinepos = passLine.length + 1;
	  return pass_noMoreTokens;
	case '!':
	  if (passLine.chars[passLinepos + 1] == '=')
	    {
	      passLinepos += 2;
	      return pass_noteq;
	    }
	  passLinepos++;
	  return pass_not;
	case '-':
	  passLinepos++;
	  return pass_hyphen;
	case '=':
	  passLinepos++;
	  return pass_eq;
	case '<':
	  passLinepos++;
	  if (passLine.chars[passLinepos] == '=')
	    {
	      passLinepos++;
	      return pass_lteq;
	    }
	  return pass_lt;
	case '>':
	  passLinepos++;
	  if (passLine.chars[passLinepos] == '=')
	    {
	      passLinepos++;
	      return pass_gteq;
	    }
	  return pass_gt;
	case '+':
	  passLinepos++;
	  return pass_plus;
	case '(':
	  passLinepos++;
	  return pass_leftParen;
	case ')':
	  passLinepos++;
	  return pass_rightParen;
	case ',':
	  passLinepos++;
	  return pass_comma;
	case '&':
	  if (passLine.chars[passLinepos = 1] == '&')
	    {
	      passLinepos += 2;
	      return pass_and;
	    }
	  return pass_invalidToken;
	case '|':
	  if (passLine.chars[passLinepos + 1] == '|')
	    {
	      passLinepos += 2;
	      return pass_or;
	    }
	  return pass_invalidToken;
	case 'a':
	  if (passIsKeyword ("ttr"))
	    return pass_attributes;
	  passGetName ();
	  return pass_nameFound;
	case 'b':
	  if (passIsKeyword ("ack"))
	    return pass_lookback;
	  if (passIsKeyword ("ool"))
	    return pass_boolean;
	  passGetName ();
	  return pass_nameFound;
	case 'c':
	  if (passIsKeyword ("lass"))
	    return pass_class;
	  passGetName ();
	  return pass_nameFound;
	case 'd':
	  if (passIsKeyword ("ef"))
	    return pass_define;
	  passGetName ();
	  return pass_nameFound;
	case 'e':
	  if (passIsKeyword ("mph"))
	    return pass_emphasis;
	  passGetName ();
	  return pass_nameFound;
	case 'f':
	  if (passIsKeyword ("ind"))
	    return pass_search;
	  if (passIsKeyword ("irst"))
	    return pass_first;
	  passGetName ();
	  return pass_nameFound;
	case 'g':
	  if (passIsKeyword ("roup"))
	    return pass_group;
	  passGetName ();
	  return pass_nameFound;
	case 'i':
	  if (passIsKeyword ("f"))
	    return pass_if;
	  passGetName ();
	  return pass_nameFound;
	case 'l':
	  if (passIsKeyword ("ast"))
	    return pass_last;
	  passGetName ();
	  return pass_nameFound;
	case 'm':
	  if (passIsKeyword ("ark"))
	    return pass_mark;
	  passGetName ();
	  return pass_nameFound;
	case 'r':
	  if (passIsKeyword ("epgroup"))
	    return pass_repGroup;
	  if (passIsKeyword ("epcopy"))
	    return pass_copy;
	  if (passIsKeyword ("epomit"))
	    return pass_omit;
	  if (passIsKeyword ("ep"))
	    return pass_replace;
	  passGetName ();
	  return pass_nameFound;
	case 's':
	  if (passIsKeyword ("cript"))
	    return pass_script;
	  if (passIsKeyword ("wap"))
	    return pass_swap;
	  passGetName ();
	  return pass_nameFound;
	case 't':
	  if (passIsKeyword ("hen"))
	    return pass_then;
	  passGetName ();
	  return pass_nameFound;
	default:
	  if (passLine.chars[passLinepos] <= 32)
	    {
	      passLinepos++;
	      break;
	    }
	  if (passLine.chars[passLinepos] >= '0'
	      && passLine.chars[passLinepos] <= '9')
	    {
	      passGetNumber ();
	      return pass_numberFound;
	    }
	  else
	    {
	      if (!passGetName ())
		return pass_invalidToken;
	      else
		return pass_nameFound;
	    }
	}
    }
  return pass_noMoreTokens;
}