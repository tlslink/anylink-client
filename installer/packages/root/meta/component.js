function Component()
{
    // end with "/" in config.xml
    var targetDir = installer.value("TargetDir");
    var uninstaller = installer.value("MaintenanceToolName");

    if (systemInfo.kernelType === "linux") {
        installer.setValue("TargetDir", targetDir + "anylink");

        component.addStopProcessForUpdateRequest("anylink");
        component.addStopProcessForUpdateRequest("vpnui");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller;

    } else if (systemInfo.kernelType === "winnt") {
        installer.setValue("TargetDir", targetDir + "AnyLink");

        component.addStopProcessForUpdateRequest("anylink.exe");
        component.addStopProcessForUpdateRequest("vpnui.exe");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller + ".exe";
    }
    if (installer.fileExists(uninstaller)) {
        installer.executeDetached(uninstaller);
    }
}

Component.prototype.createOperations = function()
{
    // default call createOperationsForArchive and then createOperationsForPath
    // The default implementation is recursively creating Copy and Mkdir operations for all files and folders within path.
    component.createOperations();
    if (systemInfo.kernelType === "linux") {
        // will be auto removed on uninstall
        component.addOperation("Copy", "@TargetDir@/anylink.desktop", "/usr/share/applications/anylink.desktop");

        // install and start the service or stop and remove the service
        component.addElevatedOperation("Execute", "@TargetDir@/bin/vpnagent","install",
                                "UNDOEXECUTE","@TargetDir@/bin/vpnagent","uninstall");
    } else if (systemInfo.kernelType === "winnt") {
        //开始菜单快捷方式
        component.addOperation("CreateShortcut",
                               "@TargetDir@/anylink.exe",
                               "@StartMenuDir@/AnyLink Secure Client.lnk",
                               "workingDirectory=@TargetDir@");

        //桌面快捷方式
        component.addOperation("CreateShortcut",
                               "@TargetDir@/anylink.exe",
                               "@DesktopDir@/AnyLink Secure Client.lnk",
                               "workingDirectory=@TargetDir@");


        component.addElevatedOperation("Execute", "@TargetDir@/vpnagent.exe","install",
                                "UNDOEXECUTE","@TargetDir@/vpnagent.exe","uninstall");
    }
}

