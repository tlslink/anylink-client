function Component()
{
    component.setUpdateAvailable(false);

    // end with "/" in config.xml
    var targetDir = installer.value("TargetDir");
    var uninstaller = installer.value("MaintenanceToolName");

    if (systemInfo.kernelType === "linux") {
        installer.setValue("TargetDir", targetDir + "anylink");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller;

    } else if (systemInfo.kernelType === "winnt") {
        installer.setValue("TargetDir", targetDir + "AnyLink");

        uninstaller = installer.value("TargetDir") + "/" + uninstaller + ".exe";
    }

    if (installer.isInstaller()) {
//         console.log("anylink running status: " + installer.isProcessRunning("anylink"))
//         console.log("vpnui running status: " + installer.isProcessRunning("vpnui"))

        // request when component really installing
        component.addStopProcessForUpdateRequest("vpnui");
    }
    // kill self when install and uninstall
    component.addStopProcessForUpdateRequest("anylink");

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
        // https://forum.qt.io/topic/87431/how-could-we-detect-users-vcredist-installed-when-using-qt-installer-framework/4
        component.addElevatedOperation("Execute", "{0,3010,1638,5100}", "@TargetDir@/vc_redist.x64.exe", "/norestart", "/q");

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

