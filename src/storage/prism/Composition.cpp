#include "src/storage/prism/Composition.h"

namespace storm {
    namespace prism {
        
        std::ostream& operator<<(std::ostream& stream, Composition const& composition) {
            composition.writeToStream(stream);
            return stream;
        }
        
    }
}
