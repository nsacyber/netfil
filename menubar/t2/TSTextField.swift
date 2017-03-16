import Foundation
import AppKit


class TSTextFieldCell : NSTextFieldCell {
    
    var leftPadding : CGFloat = 0
    var rightPadding : CGFloat = 15
    
    convenience init(leftPadding : CGFloat, rightPadding : CGFloat) {
        self.init()
        self.leftPadding = leftPadding
        self.rightPadding = rightPadding
    }
    
    override func drawingRect(forBounds rect: NSRect) -> NSRect {
        var r = rect
        r.origin.x = r.origin.x + leftPadding
        r.size.width = r.size.width - leftPadding - rightPadding
        return super.drawingRect(forBounds: r)
    }
}

class TSTextField : NSTextField {
    
    var isEditing : Bool = false
    var segmentControl : NSSegmentedControl?
    var segmentControlIndex : Int = 1
    
    override func becomeFirstResponder() -> Bool {
        
        let orig = super.becomeFirstResponder()
        Swift.print("did come")
        if(orig) {
            self.isEditing = true
            if(segmentControl != nil) {
                if((segmentControl?.segmentCount)! > segmentControlIndex) {
                    segmentControl?.setEnabled(true, forSegment: segmentControlIndex)
                }
            }
        }
        return orig
    }

    override func textDidEndEditing(_ notification: Notification) {
        Swift.print("did end")
        super.textDidEndEditing(notification)
        self.isEditing = false
        if(segmentControl != nil) {
            if((segmentControl?.segmentCount)! > segmentControlIndex) {
                segmentControl?.setEnabled(false, forSegment: segmentControlIndex)
            }
        }
    }
    
    override func selectText(_ sender: Any?) {
        super.selectText(sender)
        self.isEditing = true
        if(segmentControl != nil) {
            if((segmentControl?.segmentCount)! > segmentControlIndex) {
                segmentControl?.setEnabled(true, forSegment: segmentControlIndex)
            }
        }
    }
}
