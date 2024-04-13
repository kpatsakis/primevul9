void CClient::EnterGame()
{
	if(State() == IClient::STATE_DEMOPLAYBACK)
		return;

	// now we will wait for two snapshots
	// to finish the connection
	SendEnterGame();
	OnEnterGame();
}