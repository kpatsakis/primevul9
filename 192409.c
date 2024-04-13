static char *JSON_parse_array(JSON_Parser *json, char *p, char *pe, VALUE *result, int current_nesting)
{
    int cs = EVIL;
    VALUE array_class = json->array_class;

    if (json->max_nesting && current_nesting > json->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", current_nesting);
    }
    *result = NIL_P(array_class) ? rb_ary_new() : rb_class_new_instance(0, 0, array_class);


#line 1172 "parser.c"
	{
	cs = JSON_array_start;
	}

#line 411 "parser.rl"

#line 1179 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 91 )
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
		case 13: goto st2;
		case 32: goto st2;
		case 34: goto tr2;
		case 45: goto tr2;
		case 47: goto st13;
		case 73: goto tr2;
		case 78: goto tr2;
		case 91: goto tr2;
		case 93: goto tr4;
		case 102: goto tr2;
		case 110: goto tr2;
		case 116: goto tr2;
		case 123: goto tr2;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) >= 9 )
		goto st2;
	goto st0;
tr2:
#line 375 "parser.rl"
	{
        VALUE v = Qnil;
        char *np = JSON_parse_value(json, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 3; goto _out;}
        } else {
            if (NIL_P(json->array_class)) {
                rb_ary_push(*result, v);
            } else {
                rb_funcall(*result, i_leftshift, 1, v);
            }
            {p = (( np))-1;}
        }
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 1238 "parser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st3;
		case 44: goto st4;
		case 47: goto st9;
		case 93: goto tr4;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st3;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	switch( (*p) ) {
		case 13: goto st4;
		case 32: goto st4;
		case 34: goto tr2;
		case 45: goto tr2;
		case 47: goto st5;
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
		goto st4;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	switch( (*p) ) {
		case 42: goto st6;
		case 47: goto st8;
	}
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 42 )
		goto st7;
	goto st6;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 42: goto st7;
		case 47: goto st4;
	}
	goto st6;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 10 )
		goto st4;
	goto st8;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	switch( (*p) ) {
		case 42: goto st10;
		case 47: goto st12;
	}
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 42 )
		goto st11;
	goto st10;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 42: goto st11;
		case 47: goto st3;
	}
	goto st10;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 10 )
		goto st3;
	goto st12;
tr4:
#line 390 "parser.rl"
	{ p--; {p++; cs = 17; goto _out;} }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 1345 "parser.c"
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 42: goto st14;
		case 47: goto st16;
	}
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 42 )
		goto st15;
	goto st14;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 42: goto st15;
		case 47: goto st2;
	}
	goto st14;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) == 10 )
		goto st2;
	goto st16;
	}
	_test_eof2: cs = 2; goto _test_eof;
	_test_eof3: cs = 3; goto _test_eof;
	_test_eof4: cs = 4; goto _test_eof;
	_test_eof5: cs = 5; goto _test_eof;
	_test_eof6: cs = 6; goto _test_eof;
	_test_eof7: cs = 7; goto _test_eof;
	_test_eof8: cs = 8; goto _test_eof;
	_test_eof9: cs = 9; goto _test_eof;
	_test_eof10: cs = 10; goto _test_eof;
	_test_eof11: cs = 11; goto _test_eof;
	_test_eof12: cs = 12; goto _test_eof;
	_test_eof17: cs = 17; goto _test_eof;
	_test_eof13: cs = 13; goto _test_eof;
	_test_eof14: cs = 14; goto _test_eof;
	_test_eof15: cs = 15; goto _test_eof;
	_test_eof16: cs = 16; goto _test_eof;

	_test_eof: {}
	_out: {}
	}

#line 412 "parser.rl"

    if(cs >= JSON_array_first_final) {
        return p + 1;
    } else {
        rb_enc_raise(EXC_ENCODING eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}