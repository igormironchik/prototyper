
function Component()
{
}

Component.prototype.createOperations = function()
{
	component.createOperations();

	if( installer.value( "os" ) === "win" )
	{
		component.addOperation( "Execute", "{0,1638}", "@TargetDir@\\VC_redist.x64.exe",
			"/silent" );

		component.addOperation( "CreateShortcut", "@TargetDir@\\Prototyper.exe",
			"@StartMenuDir@\\Prototyper.lnk", "workingDirectory=@TargetDir@" );

	}
}
