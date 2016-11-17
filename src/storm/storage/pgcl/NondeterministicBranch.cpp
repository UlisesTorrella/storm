#include "storm/storage/pgcl/NondeterministicBranch.h"
#include "storm/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        NondeterministicBranch::NondeterministicBranch(std::shared_ptr<storm::pgcl::PgclBlock> const& left, std::shared_ptr<storm::pgcl::PgclBlock> const& right) {
            leftBranch = left;
            rightBranch = right;
        }

        void NondeterministicBranch::accept(storm::pgcl::AbstractStatementVisitor& visitor) {
            visitor.visit(*this);
        }

        bool NondeterministicBranch::isNondet() const {
            return true;
        }
    }
}

