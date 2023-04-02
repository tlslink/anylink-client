function Controller()
{
    installer.setDefaultPageVisible(QInstaller.ComponentSelection,false)
    installer.finishButtonClicked.connect(this, Controller.prototype.onInstallationFinished);
}

Controller.prototype.IntroductionPageCallback = function()
{
    var widget = gui.currentPageWidget()
    if (installer.isUninstaller()) {
        var buttonManager = gui.findChild(widget,"PackageManagerRadioButton")
        var buttonUpdater = gui.findChild(widget,"UpdaterRadioButton")
        buttonManager.setVisible(false)
        buttonUpdater.setVisible(false)
    }
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
    var widget = gui.currentPageWidget()
    widget.TargetDirectoryLineEdit.setEnabled(false)
    widget.BrowseDirectoryButton.setEnabled(false)
}

Controller.prototype.onInstallationFinished = function ()
{
    try {
        if (installer.isInstaller() && installer.status === QInstaller.Success) {
            var exeName = "anylink"
            if (systemInfo.kernelType === "winnt") {
                exeName += ".exe"
            }
            installer.executeDetached("@TargetDir@/bin/" + exeName);
            // QDesktopServices.openUrl("file:///" + installer.value("TargetDir") + "/logo.png");
        }
    } catch(e) {
        console.log(e);
    }
}
