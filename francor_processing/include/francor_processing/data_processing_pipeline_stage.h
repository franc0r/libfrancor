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

template <class InputType, class OutputType>
class DataInputOutput
{
protected:
  DataInputOutput() = delete;
  DataInputOutput(const std::size_t numOfInputs, const std::size_t numOfOutputs)
    : _input_ports(numOfInputs),
      _output_ports(numOfOutputs)
  {  }
  virtual ~DataInputOutput() = default;

public:
  DataInputOutput(const DataInputOutput&) = delete;

  DataInputOutput& operator=(const DataInputOutput&) = delete;

  inline InputType& input(const std::string& portName) { return this->findInput(portName); }
  inline InputType& input(const std::size_t index) { return _input_ports[index]; }
  inline const InputType& input(const std::size_t index) const { return _input_ports[index]; }
  inline OutputType& output(const std::string& portName) { return this->findOutput(portName); }
  inline OutputType& output(const std::size_t index) { return _output_ports[index]; }

protected:
  inline std::vector<InputType>& getInputs() { return _input_ports; }
  inline std::vector<OutputType>& getOutputs() { return _output_ports; }

  virtual bool initializePorts() = 0;

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

  std::vector<InputType> _input_ports; // TODO: I don't like to use heap memory here.
  std::vector<OutputType> _output_ports;
};

template <typename DataStructureType>
class ProcessingStage : public DataInputOutput<data::InputPort, data::OutputPort>
{
protected:
  ProcessingStage() = delete;
  ProcessingStage(const std::string& stageName, const std::size_t numOfInputs, const std::size_t numOfOutputs)
    : DataInputOutput<data::InputPort, data::OutputPort>(numOfInputs, numOfOutputs), _name(stageName)
  { }

public:
  virtual ~ProcessingStage() = default;

  bool process(DataStructureType& data)
  {
    base::LogDebug() << "ProcessingStage (name = " << _name << "): processing...";
    
    // first check if this stage is ready for processing
    if (!this->isReady())
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): stage is not ready for processing. Cancel it.";
      // TODO: throw exception
      return false;
    }
    // check if input data are valid
    if (!this->validateInputData())
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): input data isn't valid. Cancel processing.";
      // TODO: throw exception
      return false;
    }
    // check if data structure data is valid
    if (!this->isDataConsistant(data))
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): data structure is not consistent. Cancel processing.";
      // TODO: throw exception
      return false;
    }

    // process
    if (!this->doProcess(data))
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): error occurred during processing.";
      // TODO: throw exception
      return false;
    }

    // check if output data are valid
    if (!this->validateOutputData())
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): output data aren't valid.";
      // TODO: throw exception
      return false;
    }
    // TODO: add diagnostic

    if (!this->isDataConsistant(data))
    {
      base::LogError() << "ProcessingStage (name = " << _name << "): data structure is not consistent. Cancel processing.";
      // TODO: throw exception
      return false;
    }

    return true;
  }
  bool initialize()
  {
    base::LogDebug() << "ProcessingStage (name = " << _name << "): initializing...";

    bool ret = true;

    // initialize all data ports
    ret &= this->initializePorts();
    // initialize the stage
    ret &= this->doInitialization();

    if (ret) base::LogDebug() << "ProcessingStage (name = " << _name << "): initialization was successful.";
    else base::LogError() << "ProcessingStage (name = " << _name << "): error occurred during initialization.";

    return ret;
  }

  inline const std::string& name() const noexcept { return _name; }

protected:
  virtual bool doProcess(DataStructureType& data) = 0;
  virtual bool doInitialization() = 0;
  virtual bool isReady() const = 0;
  virtual bool validateInputData() const { return true; }
  virtual bool validateOutputData() const { return true; }
  virtual bool isDataConsistant(const DataStructureType&) const { return true; }
  
private:
  const std::string _name;
};

using NoDataType = bool;

} // end namespace processing

} // end namespace francor
