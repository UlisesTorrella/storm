#include "storm/solver/NativeLinearEquationSolver.h"

#include <utility>

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>::NativeLinearEquationSolverSettings() {
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel) {
                method = SolutionMethod::GaussSeidel;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi) {
                method = SolutionMethod::Jacobi;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::SOR) {
                method = SolutionMethod::SOR;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::WalkerChae) {
                method = SolutionMethod::WalkerChae;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Power) {
                method = SolutionMethod::Power;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The selected solution technique is invalid for this solver.");
            }
            
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            omega = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getOmega();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setOmega(ValueType omega) {
            this->omega = omega;
        }
        
        template<typename ValueType>
        typename NativeLinearEquationSolverSettings<ValueType>::SolutionMethod NativeLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return method;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getOmega() const {
            return omega;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
            clearCache();
        }

        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType>&& A) {
            localA = std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A));
            this->A = localA.get();
            clearCache();
        }

        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsSOR(std::vector<ValueType>& x, std::vector<ValueType> const& b, ValueType const& omega) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Gauss-Seidel, SOR omega = " << omega << ")");

            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;
            
            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations()) {
                A->performSuccessiveOverRelaxationStep(omega, x, b);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*this->cachedRowVector, x, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion()) || (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x));
                
                // If we did not yet converge, we need to backup the contents of x.
                if (!converged) {
                    *this->cachedRowVector = x;
                }
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterationCount << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterationCount << " iterations.");
            }
            
            return converged;
        }
    
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsJacobi(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Jacobi)");
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            // Get a Jacobi decomposition of the matrix A.
            if (!jacobiDecomposition) {
                jacobiDecomposition = std::make_unique<std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>>>(A->getJacobiDecomposition());
            }
            storm::storage::SparseMatrix<ValueType> const& jacobiLU = jacobiDecomposition->first;
            std::vector<ValueType> const& jacobiD = jacobiDecomposition->second;
            
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = this->cachedRowVector.get();
            
            // Set up additional environment variables.
            uint_fast64_t iterationCount = 0;
            bool converged = false;

            while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                // Compute D^-1 * (b - LU * x) and store result in nextX.
                multiplier.multAdd(jacobiLU, *currentX, nullptr, *nextX);

                storm::utility::vector::subtractVectors(b, *nextX, *nextX);
                storm::utility::vector::multiplyVectorsPointwise(jacobiD, *nextX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                
                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterationCount;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *currentX);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterationCount << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterationCount << " iterations.");
            }
            
            return converged;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::WalkerChaeData::WalkerChaeData(storm::storage::SparseMatrix<ValueType> const& originalMatrix, std::vector<ValueType> const& originalB) : t(storm::utility::convertNumber<ValueType>(1000.0)) {
            computeWalkerChaeMatrix(originalMatrix);
            computeNewB(originalB);
            precomputeAuxiliaryData();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::computeWalkerChaeMatrix(storm::storage::SparseMatrix<ValueType> const& originalMatrix) {
            storm::storage::BitVector columnsWithNegativeEntries(originalMatrix.getColumnCount());
            ValueType zero = storm::utility::zero<ValueType>();
            for (auto const& e : originalMatrix) {
                if (e.getValue() < zero) {
                    columnsWithNegativeEntries.set(e.getColumn());
                }
            }
            std::vector<uint64_t> columnsWithNegativeEntriesBefore = columnsWithNegativeEntries.getNumberOfSetBitsBeforeIndices();
            
            // We now build an extended equation system matrix that only has non-negative coefficients.
            storm::storage::SparseMatrixBuilder<ValueType> builder;
            
            uint64_t row = 0;
            for (; row < originalMatrix.getRowCount(); ++row) {
                for (auto const& entry : originalMatrix.getRow(row)) {
                    if (entry.getValue() < zero) {
                        builder.addNextValue(row, originalMatrix.getRowCount() + columnsWithNegativeEntriesBefore[entry.getColumn()], -entry.getValue());
                    } else {
                        builder.addNextValue(row, entry.getColumn(), entry.getValue());
                    }
                }
            }
            ValueType one = storm::utility::one<ValueType>();
            for (auto column : columnsWithNegativeEntries) {
                builder.addNextValue(row, column, one);
                builder.addNextValue(row, originalMatrix.getRowCount() + columnsWithNegativeEntriesBefore[column], one);
                ++row;
            }
            
            matrix = builder.build();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::computeNewB(std::vector<ValueType> const& originalB) {
            b = std::vector<ValueType>(originalB);
            b.resize(matrix.getRowCount());
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::WalkerChaeData::precomputeAuxiliaryData() {
            columnSums = std::vector<ValueType>(matrix.getColumnCount());
            for (auto const& e : matrix) {
                columnSums[e.getColumn()] += e.getValue();
            }
            
            newX.resize(matrix.getRowCount());
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsWalkerChae(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (WalkerChae)");
            
            // (1) Compute an equivalent equation system that has only non-negative coefficients.
            if (!walkerChaeData) {
                walkerChaeData = std::make_unique<WalkerChaeData>(*this->A, b);
            }

            // (2) Enlarge the vectors x and b to account for additional variables.
            x.resize(walkerChaeData->matrix.getRowCount());
            
            // Square the error bound, so we can use it to check for convergence. We take the squared error, because we
            // do not want to compute the root in the 2-norm computation.
            ValueType squaredErrorBound = storm::utility::pow(this->getSettings().getPrecision(), 2);
            
            // Set up references to the x-vectors used in the iteration loop.
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = &walkerChaeData->newX;

            std::vector<ValueType> tmp = walkerChaeData->matrix.getRowSumVector();
            storm::utility::vector::applyPointwise(tmp, walkerChaeData->b, walkerChaeData->b, [this] (ValueType const& first, ValueType const& second) { return walkerChaeData->t * first + second; } );
            
            // Add t to all entries of x.
            storm::utility::vector::applyPointwise(x, x, [this] (ValueType const& value) { return value + walkerChaeData->t; });

            // Create a vector that always holds Ax.
            std::vector<ValueType> currentAx(x.size());
            multiplier.multAdd(walkerChaeData->matrix, *currentX, nullptr, currentAx);
            
            // (3) Perform iterations until convergence.
            bool converged = false;
            uint64_t iterations = 0;
            while (!converged && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Perform one Walker-Chae step.
                walkerChaeData->matrix.performWalkerChaeStep(*currentX, walkerChaeData->columnSums, walkerChaeData->b, currentAx, *nextX);
                
                // Compute new Ax.
                multiplier.multAdd(walkerChaeData->matrix, *nextX, nullptr, currentAx);
                
                // Check for convergence.
                converged = storm::utility::vector::computeSquaredNorm2Difference(currentAx, walkerChaeData->b) <= squaredErrorBound;
                
                // Swap the x vectors for the next iteration.
                std::swap(currentX, nextX);
                
                // Increase iteration count so we can abort if convergence is too slow.
                ++iterations;
            }
            
            // If the last iteration did not write to the original x we have to swap the contents, because the
            // output has to be written to the input parameter x.
            if (currentX == &walkerChaeData->newX) {
                std::swap(x, *currentX);
            }

            // Resize the solution to the right size.
            x.resize(this->A->getRowCount());
            
            // Finalize solution vector.
            storm::utility::vector::applyPointwise(x, x, [this] (ValueType const& value) { return value - walkerChaeData->t; } );
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }

            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterations << " iterations.");
            }

            return converged;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationsPower(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // FIXME: This solver will not work for all input systems. More concretely, the current implementation will
            // not work for systems that have a 0 on the diagonal. This is not a restriction of this technique in general
            // but arbitrary matrices require pivoting, which is not currently implemented.
            STORM_LOG_INFO("Solving linear equation system (" << x.size() << " rows) with NativeLinearEquationSolver (Power)");
            
            // We need to revert the transformation into an equation system matrix, because the elimination procedure
            // and the distance computation is based on the probability matrix instead.
            storm::storage::SparseMatrix<ValueType> locallyConvertedMatrix;
            if (localA) {
                localA->convertToEquationSystem();
            } else {
                locallyConvertedMatrix = *A;
                locallyConvertedMatrix.convertToEquationSystem();
            }
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix = localA ? *localA : locallyConvertedMatrix;
            
            if (!this->cachedRowVector) {
                this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
            }
            
            std::vector<ValueType>* currentX = &x;
            std::vector<ValueType>* nextX = this->cachedRowVector.get();

            bool converged = false;
            uint64_t iterations = 0;
            while (!converged && iterations < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                this->multiplier.multAdd(transitionMatrix, *currentX, &b, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                
                // Set up next iteration.
                std::swap(currentX, nextX);
                ++iterations;
            }
            
            if (currentX == this->cachedRowVector.get()) {
                std::swap(x, *nextX);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged in " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge in " << iterations << " iterations.");
            }

            return converged;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR || this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::GaussSeidel) {
                return this->solveEquationsSOR(x, b, this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR ? this->getSettings().getOmega() : storm::utility::one<ValueType>());
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Jacobi) {
                return this->solveEquationsJacobi(x, b);
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::WalkerChae) {
                return this->solveEquationsWalkerChae(x, b);
            } else if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::Power) {
                return this->solveEquationsPower(x, b);
            }
            
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unknown solving technique.");
            return false;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            if (&x != &result) {
                multiplier.multAdd(*A, x, b, result);
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
                }
                
                multiplier.multAdd(*A, x, b, *this->cachedRowVector);
                result.swap(*this->cachedRowVector);
                
                if (!this->isCachingEnabled()) {
                    clearCache();
                }
            }
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyAndReduce(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result, std::vector<uint_fast64_t>* choices) const {
            if (&x != &result) {
                multiplier.multAddReduce(dir, rowGroupIndices, *A, x, b, result, choices);
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                if (!this->cachedRowVector) {
                    this->cachedRowVector = std::make_unique<std::vector<ValueType>>(getMatrixRowCount());
                }
            
                multiplier.multAddReduce(dir, rowGroupIndices, *A, x, b, *this->cachedRowVector, choices);
                result.swap(*this->cachedRowVector);
                
                if (!this->isCachingEnabled()) {
                    clearCache();
                }
            }
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::supportsGaussSeidelMultiplication() const {
            return true;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyGaussSeidel(std::vector<ValueType>& x, std::vector<ValueType> const* b) const {
            STORM_LOG_ASSERT(this->A->getRowCount() == this->A->getColumnCount(), "This function is only applicable for square matrices.");
            multiplier.multAddGaussSeidelBackward(*A, x, b);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiplyAndReduceGaussSeidel(OptimizationDirection const& dir, std::vector<uint64_t> const& rowGroupIndices, std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<uint_fast64_t>* choices) const {
            multiplier.multAddReduceGaussSeidelBackward(dir, rowGroupIndices, *A, x, b, choices);
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setSettings(NativeLinearEquationSolverSettings<ValueType> const& newSettings) {
            settings = newSettings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::clearCache() const {
            jacobiDecomposition.reset();
            walkerChaeData.reset();
            LinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return this->A->getRowCount();
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return this->A->getColumnCount();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(matrix, settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(std::move(matrix), settings);
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>& NativeLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> NativeLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolverSettings<double>;
        template class NativeLinearEquationSolver<double>;
        template class NativeLinearEquationSolverFactory<double>;
    }
}
