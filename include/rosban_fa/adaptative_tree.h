#pragma once

#include "rosban_fa/optimizer_trainer.h"

/// This classes uses the following concepts:
/// - A set of samples is used to build tree-based approximations
/// - The set of samples can only be splitted on medians and is always splitted
///   on the best median
/// - Evaluation of interpolation methods is performed through cross-validation,
///   in order to avoid over-fitting (on generated states)
/// - At each step, multiple models are fitted and the best one is chosen
///   (according to cross-validation) 
/// - Multiple generations can be used, in this case each generation is based on
///   the previous one and has more chance to optimize samples in areas where
///   the loss due to use of the approximator is expected to be the highest
class AdaptativeTree : public rosban_fa::OptimizerTrainer
{
public:

  AdaptativeTree();
  virtual ~AdaptativeTree();

  virtual std::unique_ptr<FunctionApproximator>
  train(RewardFunction rf, std::default_random_engine * engine);

  /// Generate a new set of samples:
  /// - Using uniformous random for first generation
  /// - Based on processed leaves on further generation
  void generateParametersSet(std::default_random_engine * engine);

  /// Initialize the tree for the provide set of samples
  /// - generate the set of samples to be used for the generation
  /// - use recursion to determine the complete FunctionApproximator
  std::unique_ptr<FunctionApproximator> runGeneration(std::default_random_engine * engine);

  /// Treat the pending leaf provided:
  /// - Test multiple split options
  ///   - If a split improves the cross-validation
  ///     - Splits the node and add the two created leaves to the pending_leaves
  ///   - If no split has been found improving cross-validation
  ///     - Does not add any leaves to pending_leaves
  void treatLeaf(PendingLeaf & leaf, std::default_random_engine * engine);


  /// Use an initial guess with its own value and if splitting improves, then split it
  /// - Add leafs created to processed leafs
  /// - Return the function approximator chosen by candidate
  /// - Might consume candidate.approximator
  std::unique_ptr<FunctionApproximator> buildApproximator(ApproximatorCandidate & candidate,
                                                          std::default_random_engine * engine);

  /// Compute the split candidates from a Matrix in which:
  /// - Lines are dimensions
  /// - Each column is one of the samples
  std::vector<std::unique_ptr<Split>> getSplitCandidates(const Eigen::MatrixXd & samples);

  /// Build a cross-validation state for the given space and the given training_set_size
  Eigen::MatrixXd getCrossValidationSet(const Eigen::MatrixXd & space,
                                        int training_set_size,
                                        std::default_random_engine * engine);

  /// TODO imple
  std::unique_ptr<FunctionApproximator> optimizeAction(const Eigen::MatrixXd & parameters_set,
                                                       std::default_random_engine * engine);

  /// Update the 'reward' field of the candidate, using his 'approximator'
  /// and cross-validation
  double updateReward(ApproximatorCandidate & candidate,
                      std::default_random_engine * engine);

private:
  /// TODO: describe
  struct ApproximatorCandidate
  {
    std::unique_ptr<FunctionApproximator> approximator;
    Eigen::MatrixXd parameters_set;
    Eigen::MatrixXd parameters_space;
    double reward;
  };

  /// For processed leafs, some informations are required
  /// - space: Which was the leaf space (used to draw samples of the next generation
  /// - loss: For each leaf, a loss has been estimated using cross-validation set
  /// - nb_samples: what was the count of samples used for learning in the space
  struct ProcessedLeaf
  {
    Eigen::MatrixXd space;
    double loss;
    int nb_samples;
  };

  /// Leafs which have already been processed
  std::deque<ProcessedLeaf> processed_leaves;

  /// Number of generations used for training
  int nb_generations;

  /// Numbers of samples wished
  int nb_samples;

  /// TODO: handle nb_samples growth

  /// Cross-Validation set size ratio of samples used for cross-validation
  /// Number of samples used for cross_validation is nb samples used for
  /// training times cv_ratio
  double cv_ratio;

  /// Number of evaluations required to estimate the average reward for a set:
  /// (parameter, action)
  int evaluation_trials;
};
