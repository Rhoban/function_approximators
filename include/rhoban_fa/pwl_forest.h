#pragma once

#include "rhoban_fa/forest_approximator.h"

#include <Eigen/Core>

namespace rhoban_fa
{
class PWLForest : public ForestApproximator
{
public:
  PWLForest();
  PWLForest(std::unique_ptr<Forests> forests, int max_action_tiles);

  virtual ~PWLForest();

  virtual std::unique_ptr<FunctionApproximator> clone() const override;

  virtual int getClassID() const override;
};

}  // namespace rhoban_fa
