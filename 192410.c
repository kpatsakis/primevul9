static char *JSON_parse_value(JSON_Parser *json, char *p, char *pe, VALUE *result, int current_nesting)
{
    int cs = EVIL;


#line 501 "parser.c"
	{
	cs = JSON_value_start;
	}

#line 289 "parser.rl"

#line 508 "parser.c"
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
		case 45: goto tr3;
		case 47: goto st6;
		case 73: goto st10;
		case 78: goto st17;
		case 91: goto tr7;
		case 102: goto st19;
		case 110: goto st23;
		case 116: goto st26;
		case 123: goto tr11;
	}
	if ( (*p) > 10 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) >= 9 )
		goto st1;
	goto st0;
st0:
cs = 0;
	goto _out;
tr2:
#line 234 "parser.rl"
	{
        char *np = JSON_parse_string(json, p, pe, result);
        if (np == NULL) { p--; {p++; cs = 29; goto _out;} } else {p = (( np))-1;}
    }
	goto st29;
tr3:
#line 239 "parser.rl"
	{
        char *np;
        if(pe > p + 8 && !strncmp(MinusInfinity, p, 9)) {
            if (json->allow_nan) {
                *result = CMinusInfinity;
                {p = (( p + 10))-1;}
                p--; {p++; cs = 29; goto _out;}
            } else {
                rb_enc_raise(EXC_ENCODING eParserError, "%u: unexpected token at '%s'", __LINE__, p);
            }
        }
        np = JSON_parse_float(json, p, pe, result);
        if (np != NULL) {p = (( np))-1;}
        np = JSON_parse_integer(json, p, pe, result);
        if (np != NULL) {p = (( np))-1;}
        p--; {p++; cs = 29; goto _out;}
    }
	goto st29;
tr7:
#line 257 "parser.rl"
	{
        char *np;
        np = JSON_parse_array(json, p, pe, result, current_nesting + 1);
        if (np == NULL) { p--; {p++; cs = 29; goto _out;} } else {p = (( np))-1;}
    }
	goto st29;
tr11:
#line 263 "parser.rl"
	{
        char *np;
        np =  JSON_parse_object(json, p, pe, result, current_nesting + 1);
        if (np == NULL) { p--; {p++; cs = 29; goto _out;} } else {p = (( np))-1;}
    }
	goto st29;
tr25:
#line 227 "parser.rl"
	{
        if (json->allow_nan) {
            *result = CInfinity;
        } else {
            rb_enc_raise(EXC_ENCODING eParserError, "%u: unexpected token at '%s'", __LINE__, p - 8);
        }
    }
	goto st29;
tr27:
#line 220 "parser.rl"
	{
        if (json->allow_nan) {
            *result = CNaN;
        } else {
            rb_enc_raise(EXC_ENCODING eParserError, "%u: unexpected token at '%s'", __LINE__, p - 2);
        }
    }
	goto st29;
tr31:
#line 214 "parser.rl"
	{
        *result = Qfalse;
    }
	goto st29;
tr34:
#line 211 "parser.rl"
	{
        *result = Qnil;
    }
	goto st29;
tr37:
#line 217 "parser.rl"
	{
        *result = Qtrue;
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 269 "parser.rl"
	{ p--; {p++; cs = 29; goto _out;} }
#line 628 "parser.c"
	switch( (*p) ) {
		case 13: goto st29;
		case 32: goto st29;
		case 47: goto st2;
	}
	if ( 9 <= (*p) && (*p) <= 10 )
		goto st29;
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
		case 47: goto st29;
	}
	goto st3;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st29;
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
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 110 )
		goto st11;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( (*p) == 102 )
		goto st12;
	goto st0;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 105 )
		goto st13;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 110 )
		goto st14;
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 105 )
		goto st15;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 116 )
		goto st16;
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) == 121 )
		goto tr25;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) == 97 )
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 78 )
		goto tr27;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 97 )
		goto st20;
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	if ( (*p) == 108 )
		goto st21;
	goto st0;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
	if ( (*p) == 115 )
		goto st22;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 101 )
		goto tr31;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 117 )
		goto st24;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 108 )
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 108 )
		goto tr34;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 114 )
		goto st27;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 117 )
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 101 )
		goto tr37;
	goto st0;
	}
	_test_eof1: cs = 1; goto _test_eof;
	_test_eof29: cs = 29; goto _test_eof;
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
	_test_eof19: cs = 19; goto _test_eof;
	_test_eof20: cs = 20; goto _test_eof;
	_test_eof21: cs = 21; goto _test_eof;
	_test_eof22: cs = 22; goto _test_eof;
	_test_eof23: cs = 23; goto _test_eof;
	_test_eof24: cs = 24; goto _test_eof;
	_test_eof25: cs = 25; goto _test_eof;
	_test_eof26: cs = 26; goto _test_eof;
	_test_eof27: cs = 27; goto _test_eof;
	_test_eof28: cs = 28; goto _test_eof;

	_test_eof: {}
	_out: {}
	}

#line 290 "parser.rl"

    if (cs >= JSON_value_first_final) {
        return p;
    } else {
        return NULL;
    }
}