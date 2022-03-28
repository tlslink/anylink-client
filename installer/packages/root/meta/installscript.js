function Component()
{
    if (systemInfo.kernelType === "linux") {
        if (installer.fileExists("/opt/dtlslink")) {
            installer.execute("/opt/dtlslink/uninstall");
        }
    }
    component.addStopProcessForUpdateRequest("dtlslink");
    component.addStopProcessForUpdateRequest("vpnui");

    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.beginInstallation = function()
{
    component.beginInstallation();
}


Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.kernelType === "linux") {
        component.addOperation("Copy", "@TargetDir@/dtlslink.desktop", "/usr/share/applications/dtlslink.desktop");
        component.addOperation("Copy", "@TargetDir@/vpnagent.service", "/lib/systemd/system/vpnagent.service");
        component.addOperation("Execute", "systemctl","enable", "vpnagent",
                                "UNDOEXECUTE","systemctl","disable","vpnagent");
        component.addOperation("Execute", "systemctl","start", "vpnagent","UNDOEXECUTE","systemctl","stop","vpnagent");
    }
}

Component.prototype.installationFinishedPageIsShown = function()
{
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            installer.addWizardPageItem( component, "RunAppCheckBoxForm", QInstaller.InstallationFinished );
        }
    } catch(e) {
        console.log(e);
    }
}

Component.prototype.installationFinished = function()
{
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            var checkboxForm = component.userInterface( "RunAppCheckBoxForm" );
            if (checkboxForm && checkboxForm.runAppCheckBox.checked) {
                installer.executeDetached("@TargetDir@/bin/dtlslink");
                // QDesktopServices.openUrl("file:///" + installer.value("TargetDir") + "/logo.png");
            }
        }
    } catch(e) {
        console.log(e);
    }
}

