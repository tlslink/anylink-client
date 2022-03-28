function Controller()
{ 
    installer.setDefaultPageVisible(QInstaller.ComponentSelection,false)   
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