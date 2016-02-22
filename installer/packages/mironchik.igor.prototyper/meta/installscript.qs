
function Component()
{
}

Component.prototype.createOperations = function()
{
	component.createOperations();

	if( installer.value( "os" ) === "win" )
	{
		component.addOperation( "Execute", "@TargetDir@/vcredist_x86.exe",
			"/silent" );
	}
}
