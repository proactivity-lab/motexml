configuration MLFormatC {
	
}
implementation {
	
	components MLFormatP;
	
	components LocalTimeMilliC;
	MLFormatP.LocalTime -> LocalTimeMilliC;
	
}