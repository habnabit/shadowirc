//This example is valid for ShadowIRC 0.6.0 and later.
pascal void OpAllUsers(channelPtr channel)
{
	LongString modes;
	UserListPtr u=channel->userlist;
	
	modes.len=0;
	while(u)
	{
		StackModes(&modes, channel, "\p+o", u->nick);
		u=u->next;
	}
	
	StackModes(&modes, channel, 0, 0);
}

//This was the example for 0.5.4. It is no longer needed. Don't use this.
pascal void OpAllUsers(channelPtr channel)
{
	LongString modes;
	UserListPtr u=channel->userlist;
	
	modes.len=0;
	while(u)
	{
		StackModes(&modes, channel->chName, "\p+o", u->nick);
		u=u->next;
	}
	
	if(modes.len)
	{
		LongString ls;
		LSConcatStrAndStr("\pMODE ", channel->chName, &ls);
		LSAppend1(ls, ' ');
		LSConcatLSAndLS(&ls, &modes, &ls);
		HandleCommand(&ls);
	}
	
}
