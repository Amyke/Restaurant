function Component() {}

Component.prototype.createOperations = function() {
  component.createOperations()

  component.addOperation(
    "CreateShortcut",
    "@TargetDir@/bin/restaurant-app.exe",
    "@StartMenuDir@/Restaurant Customer.lnk",
    "workingDirectory=@TargetDir@",
    "iconPath=%SystemRoot%/system32/SHELL32.dll",
    "iconId=2",
    "description=Start Restaurant Customer application"
  );
}
