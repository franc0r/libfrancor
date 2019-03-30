/**
 * Unit test for the ransac class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. March 2019
 */
#include <gtest/gtest.h>

#include <numeric>

#include "francor_algorithm/ransac.h"

using francor::algorithm::Ransac;

struct RansacDataSniffer
{
  std::vector<std::array<int, 2>> model_points;
  std::vector<std::vector<std::size_t>> found_indices;
} ransacData;

class RansacDummyModel : public francor::algorithm::RansacTargetModel<int, int, 2>
{
public:
  RansacDummyModel(void) = default;
  virtual ~RansacDummyModel(void) = default;

  virtual double error(const int& data) const override final
  {
    // return static_cast<double>(std::abs(_model - data));
    return (data == _model_points_candidate[0] || data == _model_points_candidate[1] ? 0.0 : 1.0);
  }
  virtual bool estimate(const std::array<int, 2>& modelData) override final
  {
    _model_points_candidate = modelData;
    return true;
  }
  virtual int fitData(const std::vector<int, Eigen::aligned_allocator<int>>& inputData,
                      const std::vector<std::size_t>& indices) const override final
  {

    ransacData.model_points.push_back(_model_points_candidate);
    ransacData.found_indices.push_back(indices);
    return 0;
  }                    

private:
  std::array<int, 2> _model_points_candidate;
};

TEST(Ransac, GetRandomIndices)
{
  const std::vector<int, Eigen::aligned_allocator<int>> inputData = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
  Ransac<RansacDummyModel> ransac;

  ransac.setMinNumPoints(2);
  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);

  const std::vector<int, Eigen::aligned_allocator<int>> foundModels = ransac(inputData);

  // check if each index was used once
  std::vector<unsigned int> countIndexUsage(inputData.size(), 0);

  for (const auto& indices : ransacData.model_points)
    for (const int index : indices)
      countIndexUsage[index]++;
  
  for (const auto& indices : ransacData.model_points)
  {
    std::cout << "model points: ";
    for (const int index : indices)
       std::cout << index << " ";

    std::cout << std::endl;
  }
  for (const auto& indices : ransacData.found_indices)
  {
    std::cout << "model indices: ";
    for (const auto index : indices)
       std::cout << index << " ";

    std::cout << std::endl;
  }
  for (const auto count : countIndexUsage)
  {
    EXPECT_GT(count, 0);
    EXPECT_LE(count, 1);
  }
}