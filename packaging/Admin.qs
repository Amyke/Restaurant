function Component() {}

Component.prototype.createOperations = function() {
  component.createOperations()

  component.addOperation(
    "CreateShortcut",
    "@TargetDir@/bin/restaurantAdmin-app.exe",
    "@StartMenuDir@/Restaurant Admin.lnk",
    "workingDirectory=@TargetDir@",
    "iconPath=%SystemRoot%/system32/SHELL32.dll",
    "iconId=2",
    "description=Start Restaurant Admin application"
  );
}
