#!/usr/bin/swift

import Foundation

let statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.squareLength)

func setupMenuBar() {
    if let button = statusItem.button {
        button.image = NSImage(named: NSImage.Name("iconTemplate"))
        button.action = #selector(statusItemClicked)
    }

    let menu = NSMenu()
    menu.addItem(withTitle: "Action", action: #selector(doSomething))
    menu.addItem(.separator())
    menu.addItem(withTitle: "Quit", action: #selector(quitApp))

    statusItem.menu = menu
}

@objc func statusItemClicked() {
    print("Status item clicked!")
}

@objc func doSomething() {
    print("Doing something...")
}

@objc func quitApp() {
    NSApplication.shared.terminate(self)
}

setupMenuBar()
NSApplication.shared.run()