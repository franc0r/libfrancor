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
 * Abstract class for the underlaying model of the ransac. This class provides the interface that class ransac can use it.
 */
template <typename InputDataType, typename ModelDataType, const std::size_t NumberOfRequiredData>
class RansacTargetModel
{
public:
  // defines input and output type
  struct Input
  {
    using type = InputDataType;
    static constexpr std::size_t count = NumberOfRequiredData;
  };
  struct Output
  {
    using type = ModelDataType;
  };
  
  // defaulted constructor and destructor
  RansacTargetModel(void) = default;
  virtual ~RansacTargetModel(void) = default;

  virtual double error(const typename Input::type& data) const = 0;
  virtual bool estimate(const std::array<typename Input::type, Input::count>& modelData) = 0;
  virtual typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t>& indices) const = 0;
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
    static_assert(Input::count == 2);
    _model = base::Line((modelData[1] - modelData[0]).normalized(), modelData[0]);
    return true;
  }

  virtual typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t>& indices) const override final
  {
    return { fittingLineFromPoints(inputData, indices) };
  }                             
};

/**
 * Base ransac class. This class requires a target model type. Input and output are std::vectors with deducted types by the target
 * model.
 */
template <class ModelType>
class Ransac
{
public:
  // default constructor
  Ransac(void) : _gen(_rd()) { }
  // defaulted destructor
  ~Ransac(void) = default;

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

  /**
   * \brief Performs a model search on the given input data. The operator will search until the max iterations are reached or
   *        there aren't enough data elements left.
   * 
   * \param inputData the ransac will seach on this container. If the number of elements are too less nothing will happen.
   * \return the found model stored in a vector. The vector size can be zero if nothing was found.
   */
  std::vector<typename Output::type, Eigen::aligned_allocator<typename Output::type>>
  operator()(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData)
  {
    this->prepareProcessing(inputData);
    typename Output::type model;
    std::vector<typename Output::type, Eigen::aligned_allocator<typename Output::type>> models;

    while (this->process(inputData, model))
    {
      models.push_back(model);
    }
    
    return std::move(models);
  }

  /**
   * \brief Gets the maximum number of iterations. One iteration means one try to find a model. After a model was found its
   *        starts again from zero.
   * 
   * \return Maximum number of iterations.
   */
  inline unsigned int maxIterations(void) const noexcept { return _max_iterations; }
  /**
   * \brief Gets the maximum allowed error between data and model.
   * 
   * \return Allowed maximum error that a data element is part of the model.
   */
  inline double epsilon(void) const noexcept { return _epsilon; }
  /**
   * \brief Gets the minimum number of data elements a model must match. This number is equal or greater than the minimum
   *        required data elements of the target model.
   * 
   * \return Minimum number of data elements.
   */
  inline std::size_t minNumPoints(void) const noexcept { return _min_number_points; }

  /**
   * \brief Sets the maximum number of iterations. One iteration means one try to find a model. After a model was found its
   *        starts again from zero.
   * 
   * \param value Maximum number of iterations.
   */
  inline void setMaxIterations(const unsigned int value) { _max_iterations = value; }
  /**
   * \brief Sets the maximum allowed error between data and model.
   * 
   * \param value Allowed maximum error that a data element is part of the model.
   */
  inline void setEpsilon(const double value)
  {
    if (value < 0.0)
    {
      //TODO: print error
      return;
    }

    _epsilon = value;
  }
  /**
   * \brief Sets the minimum number of data elements a model must match. This number must be equal or greater than the minimum
   *        required data elements of the target model.
   * 
   * \param value Minimum number of data elements. If value < minimum required data elements of target model then value will be
   *              rejected.
   */
  inline void setMinNumPoints(const std::size_t value)
  {
    if (value < ModelType::Input::count)
    {
      //TODO: print error
      return;
    }

    _min_number_points = value;
  }

private:
  bool process(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData, typename Output::type& foundModel)
  {
    if (inputData.size() - _count_data_used < _min_number_points)
      return false;

    std::size_t foundModelPoints = 0;
    std::vector<std::size_t> modelDataIndices;
    typename Output::type model = typename Output::type();

    for (unsigned int iteration = 0; iteration < this->maxIterations(); ++iteration)
    {
      // estimate model from input data
      this->estimateModel(inputData);

      // find model points
      modelDataIndices.clear();
      modelDataIndices.reserve(inputData.size());

      for (std::size_t i = 0; i < inputData.size(); ++i)
      {
        // skip if data is already used. The indices used for model estimation aren't skipped
        if (_mask_used_data[i])
          continue;

        // calculate the error between point and model
        // if error < epsilon then save index
        if (_target_model.error(inputData[i]) <= this->epsilon())
          modelDataIndices.push_back(i);
      }

      // if it is the best try take the result
      if (modelDataIndices.size() >= _min_number_points && modelDataIndices.size() > foundModelPoints)
      {
        foundModelPoints = modelDataIndices.size();
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

  void estimateModel(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData)
  {
      // get random indices and estimate model parameter
      const auto modelIndices = this->getNextRandomIndices();
      std::array<typename Input::type, ModelType::Input::count> data;
      assert(modelIndices.size() == data.size());

      for (std::size_t i = 0; i < data.size(); ++i)
        data[i] = inputData[modelIndices[i]];

      _target_model.estimate(data);
  }

  void confirmFoundModel(void)
  {
    // mark data as is used
    for (auto index : _index_data_to_model)
      _mask_used_data[index] = true;

    // add the number of used data elements to the counter and clear the index container
    _count_data_used += _index_data_to_model.size();
    _index_data_to_model.clear();
  }

  /**
   * Gets randomly unused indices for the next iteration. The number of indices depends on how many the model requires.
   */
  std::array<std::size_t, ModelType::Input::count> getNextRandomIndices(void)
  {
    std::array<std::size_t, ModelType::Input::count> indices;
    std::fill(indices.begin(), indices.end(), std::numeric_limits<std::size_t>::max());

    for (std::size_t index = 0; index < indices.size(); ++index)
    {
      // gets an index and check if it is already used until the required number of model points is reached
      std::size_t candidate;

      do
      {
        candidate = static_cast<std::size_t>(_dis(_gen));
      }
      while (std::find(indices.begin(), indices.end(), candidate) != indices.end()
             ||
             _mask_used_data[candidate]);

      indices[index] = candidate;
    }

    return indices;
  }

  void prepareProcessing(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData)
  {
    // clear data and reserve memory for possible count of indicies
    _mask_used_data.clear();
    _mask_used_data.resize(inputData.size(), false);

    _index_data_to_model.clear();
    _index_data_to_model.reserve(inputData.size());

    _dis = std::uniform_int_distribution<>(0, inputData.size() - 1);
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
  ModelType _target_model;
};

using LineRansac = Ransac<RansacLineModel>;

} // end namespace algorithm

} // end namespace francor