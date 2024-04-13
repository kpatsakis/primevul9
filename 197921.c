setDefaults ()
{
  if (!table->lenBeginCaps)
    table->lenBeginCaps = 2;
  makeDoubleRule (CTO_FirstWordItal, &table->lastWordItalBefore,
		  &table->firstWordItal);
  if (!table->lenItalPhrase)
    table->lenItalPhrase = 4;
  makeDoubleRule (CTO_FirstWordBold, &table->lastWordBoldBefore,
		  &table->firstWordBold);
  if (!table->lenBoldPhrase)
    table->lenBoldPhrase = 4;
  makeDoubleRule (CTO_FirstWordUnder, &table->lastWordUnderBefore,
		  &table->firstWordUnder);
  if (!table->lenUnderPhrase)
    table->lenUnderPhrase = 4;
  if (table->numPasses == 0)
    table->numPasses = 1;
  return 1;
}