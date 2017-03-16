import Foundation
import AppKit

class TSClipView : NSClipView {
    override var isFlipped: Bool {
        get {
            return true
        }
    }
}
