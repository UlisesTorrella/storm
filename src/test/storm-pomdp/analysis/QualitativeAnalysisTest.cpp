#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/api/storm.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm-pomdp/analysis/QualitativeAnalysisOnGraphs.h"
#include "storm-pomdp/transformer/MakePOMDPCanonic.h"

void graphalgorithm_test(std::string const& path, std::string const& constants, std::string formulaString) {
    storm::prism::Program program = storm::parser::PrismParser::parse(path);
    program = storm::utility::prism::preprocess(program, constants);
    std::shared_ptr<storm::logic::Formula const> formula = storm::api::parsePropertiesForPrismProgram(formulaString, program).front().getRawFormula();
    std::shared_ptr<storm::models::sparse::Pomdp<double>> pomdp = storm::api::buildSparseModel<double>(program, {formula})->as<storm::models::sparse::Pomdp<double>>();
    storm::transformer::MakePOMDPCanonic<double> makeCanonic(*pomdp);
    pomdp = makeCanonic.transform();

    // Run graph algorithm
    auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(*pomdp, *formula);
    storm::analysis::QualitativeAnalysisOnGraphs<double> qualitativeAnalysis(*pomdp);
    storm::storage::BitVector surelyNotAlmostSurelyReachTarget = qualitativeAnalysis.analyseProbSmaller1(
            formula->asProbabilityOperatorFormula());
    pomdp->getTransitionMatrix().makeRowGroupsAbsorbing(surelyNotAlmostSurelyReachTarget);
    storm::storage::BitVector targetStates = qualitativeAnalysis.analyseProb1(formula->asProbabilityOperatorFormula());
}

TEST(QualitativeAnalysis, GraphAlgorithm_Simple) {
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/simple.prism", "slippery=0.4", "Pmax=? [F \"goal\" ]");
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/simple.prism", "slippery=0.0", "Pmax=? [F \"goal\" ]");
}

TEST(QualitativeAnalysis, GraphAlgorithm_Maze) {
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/maze2.prism", "sl=0.4", "Pmax=? [F \"goal\" ]");
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/maze2.prism", "sl=0.0", "Pmax=? [F \"goal\" ]");
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/maze2.prism", "sl=0.4", "Pmax=? [!\"bad\" U \"goal\" ]");
    graphalgorithm_test(STORM_TEST_RESOURCES_DIR "/pomdp/maze2.prism", "sl=0.0", "Pmax=? [!\"bad\" U \"goal\"]");
}
