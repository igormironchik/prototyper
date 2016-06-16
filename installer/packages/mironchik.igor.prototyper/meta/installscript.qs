
function Component()
{
}

Component.prototype.createOperations = function()
{
	component.createOperations();

	if( installer.value( "os" ) === "win" )
	{
		component.addOperation( "Execute", "{0,1638}", "@TargetDir@\\vcredist_x86.exe",
			"/silent" );

		component.addOperation( "CreateShortcut", "@TargetDir@\\Prototyper.App.exe",
			"@StartMenuDir@\\Prototyper.lnk", "workingDirectory=@TargetDir@" );

	}
}
