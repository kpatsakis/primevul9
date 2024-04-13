static char *JSON_parse_string(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;
    VALUE match_string;

    *result = rb_str_buf_new(0);

#line 1520 "parser.c"
	{
	cs = JSON_string_start;
	}

#line 540 "parser.rl"
    json->memo = p;

#line 1528 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 34 )
		goto st2;
	goto st0;
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	switch( (*p) ) {
		case 34: goto tr2;
		case 92: goto st3;
	}
	if ( 0 <= (*p) && (*p) <= 31 )
		goto st0;
	goto st2;
tr2:
#line 505 "parser.rl"
	{
        *result = json_string_unescape(*result, json->memo + 1, p);
        if (NIL_P(*result)) {
            p--;
            {p++; cs = 8; goto _out;}
        } else {
            FORCE_UTF8(*result);
            {p = (( p + 1))-1;}
        }
    }
#line 516 "parser.rl"
	{ p--; {p++; cs = 8; goto _out;} }
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
#line 1571 "parser.c"
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 117 )
		goto st4;
	if ( 0 <= (*p) && (*p) <= 31 )
		goto st0;
	goto st2;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st5;
	} else
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st6;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st6;
	} else
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st7;
	} else
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st2;
	} else
		goto st2;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof;
	_test_eof8: cs = 8; goto _test_eof;
	_test_eof3: cs = 3; goto _test_eof;
	_test_eof4: cs = 4; goto _test_eof;
	_test_eof5: cs = 5; goto _test_eof;
	_test_eof6: cs = 6; goto _test_eof;
	_test_eof7: cs = 7; goto _test_eof;

	_test_eof: {}
	_out: {}
	}

#line 542 "parser.rl"

    if (json->create_additions && RTEST(match_string = json->match_string)) {
          VALUE klass;
          VALUE memo = rb_ary_new2(2);
          rb_ary_push(memo, *result);
          rb_hash_foreach(match_string, match_i, memo);
          klass = rb_ary_entry(memo, 1);
          if (RTEST(klass)) {
              *result = rb_funcall(klass, i_json_create, 1, *result);
          }
    }

    if (json->symbolize_names && json->parsing_name) {
      *result = rb_str_intern(*result);
    } else {
          if (RB_TYPE_P(*result, T_STRING)) {
              rb_str_resize(*result, RSTRING_LEN(*result));
          }
    }
    if (cs >= JSON_string_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}