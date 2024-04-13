static char *JSON_parse_object(JSON_Parser *json, char *p, char *pe, VALUE *result, int current_nesting)
{
    int cs = EVIL;
    VALUE last_name = Qnil;
    VALUE object_class = json->object_class;

    if (json->max_nesting && current_nesting > json->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", current_nesting);
    }

    *result = NIL_P(object_class) ? rb_hash_new() : rb_class_new_instance(0, 0, object_class);


#line 131 "parser.c"
	{
	cs = JSON_object_start;
	}

#line 181 "parser.rl"

#line 138 "parser.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 123 )
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
		case 47: goto st23;
		case 125: goto tr4;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st2;
	goto st0;
tr2:
#line 148 "parser.rl"
	{
        char *np;
        json->parsing_name = 1;
        np = JSON_parse_string(json, p, pe, &last_name);
        json->parsing_name = 0;
        if (np == NULL) { p--; {p++; cs = 3; goto _out;} } else {p = (( np))-1;}
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 179 "parser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st3;
		case 47: goto st4;
		case 58: goto st8;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st3;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	switch( (*p) ) {
		case 42: goto st5;
		case 47: goto st7;
	}
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 42 )
		goto st6;
	goto st5;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	switch( (*p) ) {
		case 42: goto st6;
		case 47: goto st3;
	}
	goto st5;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 10 )
		goto st3;
	goto st7;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 13: goto st8;
		case 32: goto st8;
		case 34: goto tr11;
		case 45: goto tr11;
		case 47: goto st19;
		case 73: goto tr11;
		case 78: goto tr11;
		case 91: goto tr11;
		case 102: goto tr11;
		case 110: goto tr11;
		case 116: goto tr11;
		case 123: goto tr11;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr11;
	} else if ( (*p) >= 9 )
		goto st8;
	goto st0;
tr11:
#line 133 "parser.rl"
	{
        VALUE v = Qnil;
        char *np = JSON_parse_value(json, p, pe, &v, current_nesting);
        if (np == NULL) {
            p--; {p++; cs = 9; goto _out;}
        } else {
            if (NIL_P(json->object_class)) {
                rb_hash_aset(*result, last_name, v);
            } else {
                rb_funcall(*result, i_aset, 2, last_name, v);
            }
            {p = (( np))-1;}
        }
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 266 "parser.c"
	switch( (*p) ) {
		case 13: goto st9;
		case 32: goto st9;
		case 44: goto st10;
		case 47: goto st15;
		case 125: goto tr4;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st9;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 13: goto st10;
		case 32: goto st10;
		case 34: goto tr2;
		case 47: goto st11;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st10;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 42: goto st12;
		case 47: goto st14;
	}
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 42 )
		goto st13;
	goto st12;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 42: goto st13;
		case 47: goto st10;
	}
	goto st12;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 10 )
		goto st10;
	goto st14;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 42: goto st16;
		case 47: goto st18;
	}
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) == 42 )
		goto st17;
	goto st16;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	switch( (*p) ) {
		case 42: goto st17;
		case 47: goto st9;
	}
	goto st16;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 10 )
		goto st9;
	goto st18;
tr4:
#line 156 "parser.rl"
	{ p--; {p++; cs = 27; goto _out;} }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 362 "parser.c"
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	switch( (*p) ) {
		case 42: goto st20;
		case 47: goto st22;
	}
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 42 )
		goto st21;
	goto st20;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	switch( (*p) ) {
		case 42: goto st21;
		case 47: goto st8;
	}
	goto st20;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 10 )
		goto st8;
	goto st22;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	switch( (*p) ) {
		case 42: goto st24;
		case 47: goto st26;
	}
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 42 )
		goto st25;
	goto st24;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	switch( (*p) ) {
		case 42: goto st25;
		case 47: goto st2;
	}
	goto st24;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 10 )
		goto st2;
	goto st26;
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
	_test_eof13: cs = 13; goto _test_eof;
	_test_eof14: cs = 14; goto _test_eof;
	_test_eof15: cs = 15; goto _test_eof;
	_test_eof16: cs = 16; goto _test_eof;
	_test_eof17: cs = 17; goto _test_eof;
	_test_eof18: cs = 18; goto _test_eof;
	_test_eof27: cs = 27; goto _test_eof;
	_test_eof19: cs = 19; goto _test_eof;
	_test_eof20: cs = 20; goto _test_eof;
	_test_eof21: cs = 21; goto _test_eof;
	_test_eof22: cs = 22; goto _test_eof;
	_test_eof23: cs = 23; goto _test_eof;
	_test_eof24: cs = 24; goto _test_eof;
	_test_eof25: cs = 25; goto _test_eof;
	_test_eof26: cs = 26; goto _test_eof;

	_test_eof: {}
	_out: {}
	}

#line 182 "parser.rl"

    if (cs >= JSON_object_first_final) {
        if (json->create_additions) {
            VALUE klassname;
            if (NIL_P(json->object_class)) {
              klassname = rb_hash_aref(*result, json->create_id);
            } else {
              klassname = rb_funcall(*result, i_aref, 1, json->create_id);
            }
            if (!NIL_P(klassname)) {
                VALUE klass = rb_funcall(mJSON, i_deep_const_get, 1, klassname);
                if (RTEST(rb_funcall(klass, i_json_creatable_p, 0))) {
                    *result = rb_funcall(klass, i_json_create, 1, *result);
                }
            }
        }
        return p + 1;
    } else {
        return NULL;
    }
}