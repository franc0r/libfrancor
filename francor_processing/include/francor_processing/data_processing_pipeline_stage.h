/**
 * Data processing pipeline stages.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. February 2019
 */
#pragma once

#include <francor_base/log.h>

#include "francor_processing/data_processing_port.h"

#include <string>
#include <stdexcept>

namespace francor
{

namespace processing
{

template <typename DataStructureType>
class ProcessingStage
{
protected:
  ProcessingStage() = delete;
  ProcessingStage(const std::string& stageName) : _name(stageName) { }

public:
  virtual ~ProcessingStage() = default;

  bool process(const std::shared_ptr<DataStructureType>& data)
  {
    // TODO: check input data

    bool ret = this->doProcess(data);
    // TODO: check output data
    // TODO: add diagnostic

    if (!std::is_same<DataStructureType, void>::value)
      ; // check if data is still consitent

    return ret;
  }
  bool initialize()
  {
    return this->doInitialization();
  }

  inline const std::string& name() const noexcept { return _name; }

protected:
  virtual bool doProcess(const std::shared_ptr<DataStructureType>& data) = 0;
  virtual bool doInitialization() = 0;
  
private:
  const std::string _name;
};

template <typename InputType, std::size_t NumOfInputs, typename OutputType, std::size_t NumOfOutputs>
class DataInputOutput
{
public:
  DataInputOutput() {  };
  DataInputOutput(const DataInputOutput&) = delete;
  virtual ~DataInputOutput() = default;

  DataInputOutput& operator=(const DataInputOutput&) = delete;

  InputType& input(const std::string& portName)
  {
    return this->findInput(portName);
  }
  OutputType& output(const std::string& portName)
  {
    return this->findOutput(portName);
  }
  void connectToInput(const std::string& name, OutputType& output)
  {
    auto& input = this->findInput(name);
    input.connect(output);
  }
  void connectToOutput(const std::string& name, InputType& input)
  {
    auto& output = this->findOutput(name);
    output.connect(input);
  }

protected:
  inline std::array<InputType, NumOfInputs>& getInputs() { return _input_ports; }
  inline std::array<OutputType, NumOfOutputs>& getOutputs() { return _output_ports; }

  virtual void initializePorts() = 0;

  template <typename DataType>
  inline void initializeInputPort(const std::size_t index, const std::string& name)
  {
    _input_ports[index] = InputType::template create<DataType>(name);
  }
  template <typename DataType>
  void initializeOutputPort(const std::size_t index, const std::string& name, DataType const* const data = nullptr)
  {
    _output_ports[index] = OutputType::template create(name, data);
  }
  bool initialize()
  {
    this->initializePorts();

    // for (const auto& input : _input_ports)
      ; // TODO: check if input is initialized

    // for (const auto& output : _output_ports)
      ; // TODO: check if output is initialized

    return true;
  }

private:
  InputType& findInput(const std::string& portName)
  {
    for (auto& port : _input_ports)
      if (port.name() == portName)
        return port;

    base::LogError() << "Processing Stage: input port name \"" << portName << "\" is unkown.";
    throw std::invalid_argument("Processing Stage: input port name \"" + portName + "\" is unkown.");
  }
  OutputType& findOutput(const std::string portName)
  {
    for (auto& port : _output_ports)
      if (port.name() == portName)
        return port;

    base::LogError() << "Processing Stage: output port name \"" << portName << "\" is unkown.";
    throw std::invalid_argument("Processing Stage: output port name \"" + portName + "\" is unkown.");
  }

  std::array<InputType, NumOfInputs> _input_ports;
  std::array<OutputType, NumOfOutputs> _output_ports;
};


template <std::size_t NumOfInputs, std::size_t NumOfOutputs, typename DataStructureType = void>
class ProcessingStageParent : public ProcessingStage<DataStructureType>,
                              public DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>
{
public:
  ProcessingStageParent(const std::string& name)
    : ProcessingStage<DataStructureType>(name),
      DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>() { }

  bool initialize()
  {
    bool ret = true;

    ret &= DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>::initialize();
    ret &= ProcessingStage<DataStructureType>::initialize();

    return ret;
  }
};

} // end namespace processing

} // end namespace francor
