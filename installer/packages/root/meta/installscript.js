function Component()
{
    if (systemInfo.kernelType === "linux") {
        component.addStopProcessForUpdateRequest("anylink");
        component.addStopProcessForUpdateRequest("vpnui");

        if (installer.fileExists("/opt/anylink")) {
            installer.executeDetached("/opt/anylink/uninstall");
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

        component.addOperation("Execute", "@TargetDir@/bin/vpnagent","install",
                                "UNDOEXECUTE","@TargetDir@/bin/vpnagent","uninstall");
    }
}

