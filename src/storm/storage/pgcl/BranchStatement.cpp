#include "storm/storage/pgcl/BranchStatement.h"
#include "storm/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        std::shared_ptr<storm::pgcl::PgclBlock> const& BranchStatement::getLeftBranch() const {
            return this->leftBranch;
        }

        std::shared_ptr<storm::pgcl::PgclBlock> const& BranchStatement::getRightBranch() const {
            return this->rightBranch;
        }

    }
}
