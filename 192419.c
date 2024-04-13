static VALUE cParser_parse(VALUE self)
{
  char *p, *pe;
  int cs = EVIL;
  VALUE result = Qnil;
  GET_PARSER;


#line 1861 "parser.c"
	{
	cs = JSON_start;
	}

#line 760 "parser.rl"
  p = json->source;
  pe = p + json->len;

#line 1870 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 13: goto st1;
		case 32: goto st1;
		case 34: goto tr2;
		case 45: goto tr2;
		case 47: goto st6;
		case 73: goto tr2;
		case 78: goto tr2;
		case 91: goto tr2;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st1;
	goto st0;
st0:
cs = 0;
	goto _out;
tr2:
#line 736 "parser.rl"
	{
        char *np = JSON_parse_value(json, p, pe, &result, 0);
        if (np == NULL) { p--; {p++; cs = 10; goto _out;} } else {p = (( np))-1;}
    }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 1914 "parser.c"
	switch( (*p) ) {
		case 13: goto st10;
		case 32: goto st10;
		case 47: goto st2;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st10;
	goto st0;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 42: goto st3;
		case 47: goto st5;
	}
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 42 )
		goto st4;
	goto st3;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	switch( (*p) ) {
		case 42: goto st4;
		case 47: goto st10;
	}
	goto st3;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st10;
	goto st5;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	switch( (*p) ) {
		case 42: goto st7;
		case 47: goto st9;
	}
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 42 )
		goto st8;
	goto st7;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 42: goto st8;
		case 47: goto st1;
	}
	goto st7;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 10 )
		goto st1;
	goto st9;
	}
	_test_eof1: cs = 1; goto _test_eof;
	_test_eof10: cs = 10; goto _test_eof;
	_test_eof2: cs = 2; goto _test_eof;
	_test_eof3: cs = 3; goto _test_eof;
	_test_eof4: cs = 4; goto _test_eof;
	_test_eof5: cs = 5; goto _test_eof;
	_test_eof6: cs = 6; goto _test_eof;
	_test_eof7: cs = 7; goto _test_eof;
	_test_eof8: cs = 8; goto _test_eof;
	_test_eof9: cs = 9; goto _test_eof;

	_test_eof: {}
	_out: {}
	}

#line 763 "parser.rl"

  if (cs >= JSON_first_final && p == pe) {
    return result;
  } else {
    rb_enc_raise(EXC_ENCODING eParserError, "%u: unexpected token at '%s'", __LINE__, p);
    return Qnil;
  }
}