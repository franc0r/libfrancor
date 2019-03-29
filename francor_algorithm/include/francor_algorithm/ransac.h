/**
 * Ransac classes. Just started with lines to get experience. So please respect that.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_base/line.h"
#include "francor_algorithm/geometry_fitting.h"

#include <random>
#include <vector>

namespace francor {

namespace algorithm {

/**
 * Interface class for the underlaying model of the ransac.
 */
template <typename InputDataType, typename ModelDataType, const std::size_t NumberOfRequiredData>
class RansacTargetModel
{
public:
  struct Input
  {
    using type = InputDataType;
    static constexpr std::size_t count = NumberOfRequiredData;
  };
  struct Output
  {
    using type = ModelDataType;
  };
  
  
  RansacTargetModel(void) = default;
  virtual ~RansacTargetModel(void) = default;

  virtual double error(const typename Input::type& data) const = 0;
  virtual bool estimate(const std::array<typename Input::type, Input::count>& modelData) = 0;
  virtual typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t> indices) const = 0;
  inline constexpr const typename Output::type& model(void) const noexcept { return _model; }

protected:
  typename Output::type _model;
};

/**
 * Ransac model for lines.
 */
class RansacLineModel : public RansacTargetModel<base::Vector2d, base::Line, 2>
{
public:
  RansacLineModel(void) = default;
  virtual ~RansacLineModel(void) = default;

  virtual double error(const Input::type& data) const override final
  {
    return _model.distanceTo(data);
  }

  virtual bool estimate(const std::array<Input::type, Input::count>& modelData) override final
  {
    if (modelData.size() < 2)
    {
      // TODO: print error
      return false;
    }

    _model = base::Line((modelData[1] - modelData[0]).normalized(), modelData[0]);

    return true;
  }

  virtual typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t> indices) const override final
  {
    return { fittingLineFromPoints(inputData, indices) };
  }                             
};

/**
 * Base ransac class.
 */
template <class ModelType>
class Ransac
{
protected:
  Ransac(void) = default;

public:
  // prevent copying and moving of the base class.
  Ransac(const Ransac&)
    : _gen(_rd())
  {

  }
  Ransac(Ransac&&) = delete;
  virtual ~Ransac(void) = default;

  Ransac& operator=(const Ransac&) = delete;
  Ransac& operator=(Ransac&&) = delete;

  // defines input and output type
  struct Input
  {
    using type = typename ModelType::Input::type;
  };
  struct Output
  {
    using type = typename ModelType::Output::type;
  };

  typename Output::type operator()(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData)
  {
    this->prepareProcessing(inputData);

    return { this->process(inputData) };
  }

  inline unsigned int maxIterations(void) const noexcept { return _max_iterations; }
  inline double epsilon(void) const noexcept { return _epsilon; }

protected:
  virtual typename Output::type process(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData) = 0;

  void saveDataIndex(const std::size_t index) { _index_data_to_model.push_back(index); }
  void confirmFoundModel(void)
  {
    for (auto index : _index_data_to_model)
      _mask_used_data[index] = true;

    _index_data_to_model.clear();
  }
  /**
   * Gets randomly unused indices for the next iteration. The number of indices depends on how many the model requires.
   */
  std::array<std::size_t, ModelType::Input::count> getNextRandomIndices(void)
  {
    std::array<std::size_t, ModelType::Input::count> indices;

    for (std::size_t index = 0; index < indices.size(); ++index)
    {
      // gets an index and check if it is already used until the required number of model points is reached
      std::size_t candidate;

      do
      {
        candidate = static_cast<std::size_t>(_dis(_gen));
      }
      while (std::find(indices.begin(), indices.begin() + index + 1, candidate) != indices.begin() + index + 1
             &&
             _mask_used_data[candidate] == true);

      indices[index] = candidate;
    }

    return indices;
  }

  ModelType _target_model;

private:
  void prepareProcessing(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData)
  {
    // clear data and reserve memory for possible count of indicies
    _mask_used_data.clear();
    _mask_used_data.resize(inputData.size(), false);

    _index_data_to_model.clear();
    _index_data_to_model.reserve(inputData.size());

    _dis = std::uniform_int_distribution<>(0, inputData.size());
  }

  // random number machine
  std::random_device _rd;
  std::mt19937 _gen;
  std::uniform_int_distribution<> _dis;

  // used data handling
  std::vector<std::size_t> _index_data_to_model; // TODO: maybe move it to local function scope
  std::vector<bool> _mask_used_data;

  // ransac parameters
  double _epsilon = 0.05;
  unsigned int _max_iterations = 200;
};

class LineRansac : public Ransac<RansacLineModel>
{
public:
  LineRansac(void) = default;
  LineRansac(const LineRansac&) = default;
  LineRansac(LineRansac&&) = default;
  virtual ~LineRansac(void) = default;

  LineRansac& operator=(const LineRansac&) = default;
  LineRansac& operator=(LineRansac&&) = default;

  virtual typename Output::type process(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData) override final
  {
    for (int iteration = 0; iteration < this->maxIterations(); ++iteration)
    {
      const auto modelIndices = this->getNextRandomIndices();
      std::array<typename Input::type, RansacLineModel::Input::count> data;
      assert(modelIndices.size() != data.size());

      for (std::size_t i = 0; i < data.size(); ++i)
        data[i] = inputData[modelIndices[i]];

      _target_model.estimate(data);

      // TODO: finish implementation
    }
  }
};

} // end namespace algorithm

} // end namespace francor