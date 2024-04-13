void ZrtpStateClass::evWaitClearAck(void) {
    DEBUGOUT((cout << "Checking for match in ClearAck.\n"));

//     char *msg, first, last, middle;
//     uint8_t *pkt;
// 
//     if (event->type == ZrtpPacket) {
// 	pkt = event->packet;
// 	msg = (char *)pkt + 4;
// 
// 	first = tolower(*msg);
//     middle = tolower(*(msg+4));
// 	last = tolower(*(msg+7));
// 
// 	/*
// 	 * ClearAck:
// 	 * - stop resending GoClear,
// 	 * - switch to state AckDetected, wait for peer's Hello
// 	 */
// 	if (first == 'c' && middle == 'r' && last =='k') {
// 	    cancelTimer();
// 	    sentPacket = NULL;
// 	    nextState(Initial);
// 	}
//     }
//     // Timer event triggered - this is Timer T2 to resend GoClear w/o HMAC
//     else if (event->type == Timer) {
//         if (!parent->sendPacketZRTP(sentPacket)) {
//             sendFailed();                 // returns to state Initial
//             return;
//         }
//         if (nextTimer(&T2) <= 0) {
//             timerFailed(SevereTooMuchRetries);     // returns to state Initial
//         }
//     }
//     else {  // unknown Event type for this state (covers Error and ZrtpClose)
//         if (event->type != ZrtpClose) {
//             parent->zrtpNegotiationFailed(Severe, SevereProtocolError);
//         }
// 	sentPacket = NULL;
// 	nextState(Initial);
//     }
}