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

// test datatypes for class ransac
struct RansacDataSniffer
{
  std::vector<std::array<int, 2>> model_points;
  std::vector<std::vector<std::size_t>> found_indices;
} ransacData;

/**
 * \brief Implements a dummy model for class Ransac. This model only picks two ints. The error between
 *        data and model is 0.01 if the value is equal to one of the model data, otherwise 100. This class
 *        also provides a little tracing of the Ransac::process() using the data sniffer class above.
 */
class RansacDummyModel : public francor::algorithm::RansacTargetModel<int, int, 2>
{
public:
  RansacDummyModel(void) = default;
  virtual ~RansacDummyModel(void) = default;

  virtual double error(const int& data) const override final
  {
    // return static_cast<double>(std::abs(_model - data));
    return (data == _model_points_candidate[0] || data == _model_points_candidate[1] ? 0.01 : 100.0);
  }
  virtual bool estimate(const std::array<int, 2>& modelData) override final
  {
    _model_points_candidate = modelData;
    return true;
  }
  virtual int fitData(const std::vector<int>& inputData,
                      const std::vector<std::size_t>& indices) const override final
  {

    ransacData.model_points.push_back(_model_points_candidate);
    ransacData.found_indices.push_back(indices);
    return 0;
  }                    

private:
  std::array<int, 2> _model_points_candidate;
};

/**
 * \brief Implements a dummy model for class Ransac. This model picks a single int value. If the
 *        data argument of error() is equal to the model 0.01 is returned, otherwise 100.
 */
class RansacSameValueModel : public francor::algorithm::RansacTargetModel<int, int, 1>
{
public:
  RansacSameValueModel(void) = default;
  virtual ~RansacSameValueModel(void) = default;

  virtual double error(const int& data) const override final
  {
    return (data == _model ? 0.01 : 100.0);
  }
  virtual bool estimate(const std::array<int, 1>& modelData) override final
  {
    _model = modelData[0];
    return true;
  }
  virtual int fitData(const std::vector<int>& inputData,
                      const std::vector<std::size_t>& indices) const override final
  {
    return _model;
  }        
};

TEST(Ransac, Instantiate)
{
  Ransac<RansacSameValueModel> ransac;
}

TEST(Ransac, ParameterMinNumPoints)
{
  const std::vector<int> inputData = { 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
  Ransac<RansacSameValueModel> ransac;

  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);

  // ransac has to find two models
  ransac.setMinNumPoints(5);

  EXPECT_EQ(ransac.minNumPoints(), 5);
  EXPECT_EQ(ransac(inputData).size(), 2);

  // ransac must no find a single model, because it doesn't exist 6 of each value
  ransac.setMinNumPoints(6);

  EXPECT_EQ(ransac.minNumPoints(), 6);
  EXPECT_EQ(ransac(inputData).size(), 0);

  // the min num points can be set to number less then minimum required data of the model
  ransac.setMinNumPoints(0);

  EXPECT_EQ(ransac.minNumPoints(), 6);

  // set min to the minimum required model data
  ransac.setMinNumPoints(1);

  EXPECT_EQ(ransac.minNumPoints(), 1);
  EXPECT_EQ(ransac(inputData).size(), 2);
}

TEST(Ransac, ParameterEpsilon)
{
  const std::vector<int> inputData = { 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
  Ransac<RansacSameValueModel> ransac;

  ransac.setMinNumPoints(5);
  ransac.setMaxIterations(100);

  // ransac has to find two models
  ransac.setEpsilon(0.1);

  EXPECT_EQ(ransac.epsilon(), 0.1);
  EXPECT_EQ(ransac(inputData).size(), 2);

  // ransac must not find a single model, because epsilon is to less
  ransac.setEpsilon(0.0);

  EXPECT_EQ(ransac.epsilon(), 0.0);
  EXPECT_EQ(ransac(inputData).size(), 0);

  // ransac has to find only one model, because epsilon is greater than the biggest possible error
  ransac.setEpsilon(200.0);

  EXPECT_EQ(ransac.epsilon(), 200.0);
  EXPECT_EQ(ransac(inputData).size(), 1);

  // try to set a negative epsilon
  ransac.setEpsilon(-1.0);
  EXPECT_EQ(ransac.epsilon(), 200.0);
}

TEST(Ransac, ParameterMaxIterations)
{
  const std::vector<int> inputData = { 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
  Ransac<RansacSameValueModel> ransac;

  ransac.setMinNumPoints(2);
  ransac.setEpsilon(0.1);

  // ransac has to find two models
  ransac.setMaxIterations(100);

  EXPECT_EQ(ransac.maxIterations(), 100);
  EXPECT_EQ(ransac(inputData).size(), 2);

  // ransac must no find a single model, because without a single iterations it can be found one
  ransac.setMaxIterations(0);

  EXPECT_EQ(ransac.maxIterations(), 0);
  EXPECT_EQ(ransac(inputData).size(), 0);
}

TEST(Ransac, GetRandomIndices)
{
  // this test requires that each data was found by the ransac --> num of inputData must be even
  const std::vector<int> inputData = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
  Ransac<RansacDummyModel> ransac;

  ransac.setMinNumPoints(2);
  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);

  const std::vector<int> foundModels = ransac(inputData);

  // check if each index was used once
  std::vector<unsigned int> countIndexUsage(inputData.size(), 0);

  for (const auto& indices : ransacData.model_points)
    for (const int index : indices)
      countIndexUsage[index]++;
  
  for (const auto count : countIndexUsage)
  {
    EXPECT_GT(count, 0);
    EXPECT_LE(count, 1);
  }
}

TEST(Ransac, ToLessInputData)
{
  const std::vector<int> inputData = { 0 };
  Ransac<RansacDummyModel> ransac;

  ransac.setMinNumPoints(2);
  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);

  const std::vector<int> foundModels = ransac(inputData);

  EXPECT_EQ(foundModels.size(), 0);
}

TEST(Ransac, FindThreeModels)
{
  // ransac should find three models (groups) and ignore two outliers
  const std::vector<int> inputData = { 0, 0, 0, 3, 0, 0, 5, 5, 5, 7, 5, 5, 9, 9, 9, 9, 9 };
  Ransac<RansacSameValueModel> ransac;

  ransac.setMinNumPoints(3);
  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(100);

  const std::vector<int> foundModels = ransac(inputData);

  ASSERT_EQ(foundModels.size(), 3);

  EXPECT_EQ(std::count(foundModels.begin(), foundModels.end(), 0), 1);
  EXPECT_EQ(std::count(foundModels.begin(), foundModels.end(), 5), 1);
  EXPECT_EQ(std::count(foundModels.begin(), foundModels.end(), 9), 1);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
