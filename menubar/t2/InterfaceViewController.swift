import Foundation
import AppKit

class InterfaceViewController : NSViewController {
    
    // Might want to provide a list of interfaces?
    // http://stackoverflow.com/questions/4367927/get-a-list-of-all-available-network-interfaces-en0-en1-en2-etc-with-cocoa
    
    var button : NSButton?
    var mainStackView : NSStackView?
    var all : NSTextField?
    var interfaceTextFields = [TSTextField]()
    var insideTable : NSStackView?
    
    var iByte : NSTextField?
    var oByte : NSTextField?
    var ioByte : NSTextField?
    
    
    override func loadView() {
        view = NSStackView(frame: NSRect(x: 0, y: 0, width: 500, height: 0))
        (view as! NSStackView).orientation = NSUserInterfaceLayoutOrientation.vertical
        (view as! NSStackView).spacing = CGFloat(PADDING)
        let titleBarSize = NSWindow.frameRect(forContentRect: self.view.frame, styleMask: NSWindowStyleMask.titled).height
        (view as! NSStackView).edgeInsets = EdgeInsets(top: titleBarSize, left: PADDING, bottom: PADDING, right: PADDING)
        //(view as! NSStackView).translatesAutoresizingMaskIntoConstraints = false
        
        //Interface.shared.isActive = Interface.activity.inactive
        
        //self.addObserver(Interface.isActive, forKeyPath: "status", options: 0, context: nil)
        
        //Interface.shared
        
    }
    
    override func observeValue(forKeyPath keyPath: String?, of object: Any?, change: [NSKeyValueChangeKey : Any]?, context: UnsafeMutableRawPointer?) {
        
        //super.observeValue(forKeyPath: keyPath, of: object, change: change, context: context)
        if(keyPath == "isActive") {
            statusChange()
            print("\(keyPath) did change here!", Interface.shared.isActive)
        }
    }
    
    override func viewDidLoad() {
        mainStackView = NSStackView()
        mainStackView?.orientation = NSUserInterfaceLayoutOrientation.vertical
        mainStackView?.alignment = NSLayoutAttribute.left
        mainStackView?.detachesHiddenViews = true
        mainStackView?.translatesAutoresizingMaskIntoConstraints = false
        //mainStackView?.frame.size = NSSize(width: 500, height: 50)
        //mainStackView?.setHuggingPriority(NSLayoutPriorityWindowSizeStayPut , for: NSLayoutConstraintOrientation.vertical)
        //mainStackView?.distribution = NSStackViewDistribution.fill
        
        all = NSTextField(string: "Filter All Interfaces")
        all?.alignment = NSTextAlignment.center
        all?.tag = -666;
        all?.alphaValue = 0.7
        all?.isBordered = false
        all?.isEditable = false
        all?.isSelectable = false
        all?.backgroundColor = NSColor.clear
        //mainStackView?.addArrangedSubview(all!)
        
        let table = NSScrollView(frame: NSRect(x: 0, y: 0, width: 500, height: 100))
        table.frame.size.height = 100
        insideTable = NSStackView(frame: table.frame)
        insideTable?.orientation = NSUserInterfaceLayoutOrientation.vertical
        //insideTable.alignment = NSLayoutAttribute.left
        insideTable?.translatesAutoresizingMaskIntoConstraints = false
        insideTable?.detachesHiddenViews = true
        //        for item in interfaceList {
        //            let t = NSTextField(string: item)
        //            t.placeholderString = "Interface Name"
        //            insideTable?.addArrangedSubview(t)
        //        }
        //table.documentView = insideTable
        //table.hasVerticalScroller = true
        //table.autohidesScrollers = true
        //table.translatesAutoresizingMaskIntoConstraints = false
        table.autoresizingMask = NSAutoresizingMaskOptions.viewWidthSizable
        table.contentView = TSClipView(frame: table.contentView.frame)
        table.documentView = insideTable
        
        table.borderType = NSBorderType.bezelBorder
        insideTable?.addArrangedSubview(all!)
        
        let eq = NSLayoutConstraint(item: table, attribute: NSLayoutAttribute.width, relatedBy: NSLayoutRelation.equal, toItem: insideTable!, attribute: NSLayoutAttribute.width, multiplier: 1.0, constant: 0)
        table.addConstraint(eq)
        
        mainStackView?.addArrangedSubview(table)
        
        let sc = NSSegmentedControl(images: [NSImage(named: NSImageNameAddTemplate)!, NSImage(named: NSImageNameRemoveTemplate)!, ], trackingMode: NSSegmentSwitchTracking.selectOne, target: self, action: #selector(self.didHit))
        sc.setEnabled(false, forSegment: 1)
        mainStackView?.addArrangedSubview(sc)
        
        
        button = NSButton(title: "Go!", target: self, action: #selector(buttonPress))
        
        let iByteName = NSTextField(string: "iByte:")
        iByteName.isBordered = false
        iByteName.isEditable = false
        iByteName.isSelectable = false
        iByteName.backgroundColor = NSColor.clear
        
        let iByteLabel = NSTextField(string: "bytes")
        iByteLabel.isBordered = false
        iByteLabel.isEditable = false
        iByteLabel.isSelectable = false
        iByteLabel.alphaValue = 0.7
        iByteLabel.backgroundColor = NSColor.clear
        
        iByte = NSTextField(string: String(Interface.shared.iByte))
        iByte?.placeholderString = "0"
        iByte?.alignment = NSTextAlignment.right
        
        let iByteStack = NSStackView(views: [iByteName, iByte!, iByteLabel])
        iByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        
        let oByteName = NSTextField(string: "oByte:")
        oByteName.isBordered = false
        oByteName.isEditable = false
        oByteName.isSelectable = false
        oByteName.backgroundColor = NSColor.clear
        
        let oByteLabel = NSTextField(string: "bytes")
        oByteLabel.isBordered = false
        oByteLabel.isEditable = false
        oByteLabel.isSelectable = false
        oByteLabel.alphaValue = 0.7
        oByteLabel.backgroundColor = NSColor.clear
        
        oByte = NSTextField(string: String(Interface.shared.oByte))
        oByte?.placeholderString = "0"
        oByte?.alignment = NSTextAlignment.right
        
        let oByteStack = NSStackView(views: [oByteName, oByte!, oByteLabel])
        oByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        
        let ioByteName = NSTextField(string: "ioByte:")
        ioByteName.isBordered = false
        ioByteName.isEditable = false
        ioByteName.isSelectable = false
        ioByteName.backgroundColor = NSColor.clear
        
        let ioByteLabel = NSTextField(string: "bytes")
        ioByteLabel.isBordered = false
        ioByteLabel.isEditable = false
        ioByteLabel.isSelectable = false
        ioByteLabel.wantsLayer = true // to enable transparency
        ioByteLabel.alphaValue = 0.7
        ioByteLabel.backgroundColor = NSColor.clear
        
        ioByte = TSTextField(string: String(Interface.shared.ioByte))
        ioByte?.placeholderString = "0"
        ioByte?.alignment = NSTextAlignment.right
        
        let ioByteStack = NSStackView(views: [ioByteName, ioByte!, ioByteLabel])
        ioByteStack.orientation = NSUserInterfaceLayoutOrientation.horizontal
        //ioByteStack.alignment = NSLayoutAttribute.right
        
        //let parentStackView = NSStackView(views: [titleStackView, iByteStack, oByteStack, ioByteStack, mainStackView!, button!])
        //parentStackView.orientation = NSUserInterfaceLayoutOrientation.vertical
        //parentStackView.frame.size.height = 20
        //parentStackView.frame.size.width = 200;
        //self.view = parentStackView
        (view as! NSStackView).addArrangedSubview(iByteStack)
        (view as! NSStackView).addArrangedSubview(oByteStack)
        (view as! NSStackView).addArrangedSubview(ioByteStack)
        (view as! NSStackView).addArrangedSubview(mainStackView!)
        (view as! NSStackView).addArrangedSubview(button!)
        
        let equal = NSLayoutConstraint(item: table.contentView, attribute: NSLayoutAttribute.height, relatedBy: NSLayoutRelation.equal , toItem: mainStackView!, attribute: NSLayoutAttribute.height, multiplier: 0.0, constant: 100.0)
        mainStackView?.addConstraint(equal)
        
        let equal2 = NSLayoutConstraint(item: all!, attribute: NSLayoutAttribute.width , relatedBy: NSLayoutRelation.equal , toItem: mainStackView!, attribute: NSLayoutAttribute.width, multiplier: 1.0, constant: 0.0)
        mainStackView?.addConstraint(equal2)
        
        let equal3 = NSLayoutConstraint(item: mainStackView!, attribute: NSLayoutAttribute.width , relatedBy: NSLayoutRelation.equal , toItem: self.view, attribute: NSLayoutAttribute.width, multiplier: 0.0, constant: self.view.frame.width - PADDING * 2)
        self.view.addConstraint(equal3)
        
        Interface.shared.addObserver(self, forKeyPath: "isActive", options: [NSKeyValueObservingOptions.initial, NSKeyValueObservingOptions.new], context: nil)
        
        Timer.scheduledTimer(timeInterval: 0.2, target: self, selector: #selector(checkStatus), userInfo: nil, repeats: true)
    }
    
    func buttonPress() {
        switch(Interface.shared.isActive) {
        case .active:
            Interface.shared.isActive = Interface.activity.inactive
            break
        case .inactive:
            var l = [String]()
            for i in interfaceTextFields {
                if(!i.stringValue.isEmpty) {
                    l.append(i.stringValue)
                }
            }
            Interface.shared.list = l
            Interface.shared.iByte = CInt(Int((iByte?.stringValue)!)!)
            Interface.shared.oByte = CInt(Int((oByte?.stringValue)!)!)
            Interface.shared.ioByte = CInt(Int((ioByte?.stringValue)!)!)
            Interface.shared.isActive = Interface.activity.active
            break
        default:
            break
        }
        statusChange()
    }
    
    func statusChange() {
        let s = Interface.shared.isActive
        lastStatus = s
        switch(Interface.shared.isActive) {
            case .active:
                button?.title = "Stop"
                button?.isEnabled = true
                break
            case .inactive:
                button?.title = "Filter"
                button?.isEnabled = true
                break
            default:
                button?.title = "Filter"
                button?.isEnabled = false
        }
    }
    
    var lastStatus : Interface.activity = Interface.activity.unknown
    func checkStatus() {
        
        // This is necessary cause our KVO only detects changes
        // that we make, not that the kernel makes, or other 
        // external programs
        
        if Interface.shared.isActive != lastStatus {
            statusChange()
        }
    }
    
    func didHit(_ sender : NSSegmentedControl) {
        print("did hit", sender.selectedSegment)
        if(sender.selectedSegment == 0) {
            self.addRow(sender)
        } else {
            self.removeRow(sender)
        }
        sender.setSelected(false, forSegment: sender.selectedSegment)
    }
    
    func removeRow(_ sender : NSSegmentedControl) {
        
        var didFind = false
        for (index, textField) in interfaceTextFields.enumerated() {
            if textField.isEditing {
                didFind = true
                print(textField.stringValue, "is first responder")
                textField.removeFromSuperview()
                interfaceTextFields.remove(at: index)
                
                if(interfaceTextFields.count > 0) {
                    if(interfaceTextFields.count > index) {
                        self.view.window?.makeFirstResponder(interfaceTextFields[index])
                    } else {
                        self.view.window?.makeFirstResponder(interfaceTextFields[max(index-1, 0)])
                    }
                }
                break
            }
        }
        
        if(!didFind) {
            sender.setEnabled(false, forSegment: 1)
        }
        
        if(interfaceTextFields.count <= 0) {
            all?.isHidden = false
            sender.setEnabled(false, forSegment: 1)
        }
    }
    
    var firstResponder : NSResponder? {
        get {
            return self.view.window?.firstResponder
        }
    }
    
    func addRow(_ sender : NSSegmentedControl) {
        all?.isHidden = true
        //sender.setEnabled(true, forSegment: 1)
        
        let removeButton = NSButton(image: NSImage(named: NSImageNameStopProgressFreestandingTemplate)!, target: self, action: #selector(self.removeRow))
        removeButton.imagePosition = NSCellImagePosition.imageOnly
        removeButton.isBordered = false
        
        let ifn = TSTextField()
        ifn.placeholderString = "Interface name"
        ifn.isSelectable  = true
        ifn.segmentControl = sender
        interfaceTextFields.append(ifn)
        
        let row = NSStackView()
        row.addArrangedSubview(ifn)
        //row.addArrangedSubview(removeButton)
        //let row  = NSStackView(views: [ifn, removeButton])
        //row.frame.size.width = WIDTH
        row.orientation = NSUserInterfaceLayoutOrientation.horizontal
        //row.alignment = NSLayoutAttribute.right
        
        //        let sss = NSLayoutConstraint(item: row, attribute: NSLayoutAttribute.width , relatedBy: NSLayoutRelation.equal , toItem: ifn, attribute: NSLayoutAttribute.width, multiplier: 0.5, constant: -60 )
        //        row.addConstraint(sss)
        
        
        //removeButton.tag = max((mainStackView?.arrangedSubviews.count)!-1, 0)
        insideTable?.addArrangedSubview(ifn)
        //self.view.window?.makeFirstResponder(ifn)
        ifn.selectText(self)
        
        //        if(self.view.window?.makeFirstResponder(ifn))! {
        //            ifn.isEditing = true
        //            sender.setEnabled(true, forSegment: 1)
        //        }
        //        let equal = NSLayoutConstraint(item: insideTable!, attribute: NSLayoutAttribute.width, relatedBy: NSLayoutRelation.equal , toItem: row, attribute: NSLayoutAttribute.width, multiplier: 1.0, constant: 0.0)
        //        insideTable?.addConstraint(equal)
        
    }
}
