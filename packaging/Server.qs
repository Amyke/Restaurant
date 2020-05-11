function Component() {}

Component.prototype.createOperations = function() {
  component.createOperations()

  component.addOperation(
    "CreateShortcut",
    "@TargetDir@/bin/restaurant-server.exe",
    "@StartMenuDir@/Restaurant Server.lnk",
    "workingDirectory=@TargetDir@",
    "iconPath=%SystemRoot%/system32/SHELL32.dll",
    "iconId=2",
    "description=Start Restaurant Server application"
  );
}
