function Controller()
{
    // can't use component!
    installer.setDefaultPageVisible(QInstaller.ComponentSelection,false)
    installer.finishButtonClicked.connect(this, Controller.prototype.onInstallationFinished)
//    installer.cancelMetaInfoJob()
}

Controller.prototype.IntroductionPageCallback = function()
{
//    gui.clickButton(buttons.NextButton);
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
            if (systemInfo.kernelType === "linux") {
                installer.executeDetached("@TargetDir@/bin/anylink");
            } else if (systemInfo.kernelType === "winnt") {
                installer.executeDetached("@TargetDir@/anylink.exe");
            }
            QDesktopServices.openUrl("file:///" + installer.value("TargetDir") + "/README.html");
        }
    } catch(e) {
        console.log(e);
    }
}

//Controller.prototype.onUninstallationFinished = function ()
//{
//    try {
//        if (installer.isUninstaller()) {
//            QDesktopServices.openUrl("https://anylink.pro");
//        }
//    } catch(e) {
//        console.log(e);
//    }
//}
