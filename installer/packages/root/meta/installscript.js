function Component()
{
    if (systemInfo.kernelType === "linux") {
        component.addStopProcessForUpdateRequest("anylink");
        component.addStopProcessForUpdateRequest("vpnui");

        if (installer.fileExists("@TargetDir@/@MaintenanceToolName@")) {
            installer.executeDetached("@TargetDir@/@MaintenanceToolName@");
        }
    } else if (systemInfo.kernelType === "winnt") {
        component.addStopProcessForUpdateRequest("anylink.exe");
        component.addStopProcessForUpdateRequest("vpnui.exe");

        if (installer.fileExists("@TargetDir@/@MaintenanceToolName@.exe")) {
            installer.executeDetached("@TargetDir@/@MaintenanceToolName@.exe");
        }
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

