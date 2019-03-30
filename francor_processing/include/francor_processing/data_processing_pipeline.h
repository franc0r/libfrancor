/**
 * Data processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#pragma once

#include <string>

#include "francor_base/vector.h"
#include "francor_base/line.h"

#include "francor_processing/data_processing_pipeline_port.h"

namespace francor
{

namespace processing
{

class DataProcessingStage
{
protected:
  DataProcessingStage(void) = delete;
  DataProcessingStage(const std::string& name) : _name(name) { }
  DataProcessingStage(const DataProcessingStage&) = delete;
  DataProcessingStage(DataProcessingStage&&) = delete;
  virtual ~DataProcessingStage(void) = default;

  DataProcessingStage& operator=(const DataProcessingStage&) = delete;
  DataProcessingStage& operator=(DataProcessingStage&&) = delete;

  virtual bool process(void) = 0;

  inline const std::string& name(void) const noexcept { return _name; }

private:
  const std::string _name;
};

class DetectLines : public DataProcessingStage, public InputBlock<base::VectorVector2d>, public OutputBlock<base::LineVector>
{
public:
  DetectLines(void) : DataProcessingStage("detect lines"),
                      InputBlock<base::VectorVector2d>({ PortConfig("input points") }),
                      OutputBlock<base::LineVector>({ PortConfig("output lines", &_lines) })
  { }
  virtual ~DetectLines(void) = default;

  virtual bool process(void)
  {

  }

private:
  base::LineVector _lines;
};

} // end namespace processing

} // end namespace francor
