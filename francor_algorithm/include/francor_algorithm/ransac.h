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
public:
  Ransac(void)
    : _gen(_rd())
  {

  }
  virtual ~Ransac(void) = default;

  // prevent copying and moving of the base class.
  Ransac(const Ransac&) = delete;
  Ransac(Ransac&&) = delete;

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
    // TODO: call process correctly
    return { this->process(inputData) };
  }

  inline unsigned int maxIterations(void) const noexcept { return _max_iterations; }
  inline double epsilon(void) const noexcept { return _epsilon; }

protected:
  bool process(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData, typename Output::type& foundModel)
  {
    if (inputData.size() - _count_data_used <ModelType::Input::count)
      return false;

    std::size_t foundModelPoints = 0;
    std::vector<std::size_t> modelDataIndices;
    typename Output::type model;

    for (int iteration = 0; iteration < this->maxIterations(); ++iteration)
    {
      // get random indices and estimate model parameter
      const auto modelIndices = this->getNextRandomIndices();
      std::array<typename Input::type, ModelType::Input::count> data;
      modelDataIndices.reserve(inputData.size());
      assert(modelIndices.size() != data.size());

      for (std::size_t i = 0; i < data.size(); ++i)
      {
        data[i] = inputData[modelIndices[i]];
        modelDataIndices.push_back(i);
      }

      _target_model.estimate(data);

      // find model points
      for (std::size_t i = 0; i < inputData.size(); ++i)
      {
        // skip if data is already used. The indices used for model estimation aren't skipped
        if (_mask_used_data[i])
          continue;

        // calculate the error between point and model
        if (_target_model.error(inputData[i]) <= this->epsilon())
          modelDataIndices.push_back(i);
      }

      // if it is the best try take the result
      if (modelDataIndices.size() > foundModelPoints)
      {
        foundModelPoints = modelDataIndices.size();
        _count_data_used += modelDataIndices.size();
        model = _target_model.fitData(inputData, modelDataIndices);
        _index_data_to_model = std::move(modelDataIndices);
      }
    }

    // only confirm and return model if the min number of points is reached
    if (foundModelPoints < _min_number_points)
      return false;

    this->confirmFoundModel();
    foundModel = model;
    return true;
  }

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
    _count_data_used = 0;
  }

  // random number machine
  std::random_device _rd;
  std::mt19937 _gen;
  std::uniform_int_distribution<> _dis;

  // used data handling
  std::vector<std::size_t> _index_data_to_model; // TODO: maybe move it to local function scope
  std::vector<bool> _mask_used_data;
  std::size_t _count_data_used;

  // ransac parameters
  double _epsilon = 0.05;
  unsigned int _max_iterations = 200;
  std::size_t _min_number_points = 10;
};

using LineRansac = Ransac<RansacLineModel>;

} // end namespace algorithm

} // end namespace francor