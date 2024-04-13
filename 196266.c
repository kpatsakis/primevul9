void NumberFormatTest::checkRounding(DecimalFormat* df, double base, int iterations, double increment) {
    df->setRoundingIncrement(increment);
    double lastParsed=INT32_MIN; //Intger.MIN_VALUE
    for (int i=-iterations; i<=iterations;i++) {
        double iValue=base+(increment*(i*0.1));
        double smallIncrement=0.00000001;
        if (iValue!=0) {
            smallIncrement*=iValue;
        }
        //we not only test the value, but some values in a small range around it
        lastParsed=checkRound(df, iValue-smallIncrement, lastParsed);
        lastParsed=checkRound(df, iValue, lastParsed);
        lastParsed=checkRound(df, iValue+smallIncrement, lastParsed);
    }
}