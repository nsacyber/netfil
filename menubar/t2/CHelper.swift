import Foundation

extension String {
    /** Creates an `UnsafeMutableBufferPointer<CChar>` that must be freed properly after use */
    public var UTF8CStringBuffer : UnsafeMutableBufferPointer<CChar> {
        get {
            let bufferSize = self.characters.count + 1
            let buffer = UnsafeMutableBufferPointer<CChar>(start: UnsafeMutablePointer.allocate(capacity: bufferSize), count: bufferSize)
    
            self.utf8CString.withUnsafeBufferPointer {
                    for (index, character) in $0.enumerated() {
                        buffer[index] = character
                    }
            }
            
            buffer[bufferSize - 1] = 0 // null terminate
            
            return buffer
        }
    }
}

extension CCArray {
    public mutating func initialize<C: Collection>(from collection: C) where C.Iterator.Element == String, C.IndexDistance == Int {
        let count = collection.count
        
        let buffer = UnsafeMutableBufferPointer<UnsafeMutableRawPointer?>(start: UnsafeMutablePointer.allocate(capacity: count * MemoryLayout<UnsafeMutableRawPointer>.size), count: count)
        
        for (index, element) in collection.enumerated() {
            buffer[index] = element.UTF8CStringBuffer.baseAddress.map({ .init($0) })
        }
        
        self.size = Int32(count)
        self.values = buffer.baseAddress
    }
    
    public func deallocate() {
        for element in UnsafeMutableBufferPointer(start: self.values, count: Int(self.size)) {
            free(element)
        }
        
        free(self.values)
    }
}
