#include "francor_algorithm/line_processing_pipeline.h"
#include "francor_vision/image_processing_pipeline.h"
#include "francor_processing/data_processing_pipeline.h"
#include "francor_vision/io.h"

using francor::processing::ProcessingPipeline;
using francor::algorithm::DetectLineSegments;
using francor::vision::ExportClusteredPointsFromBitMask;
using francor::vision::ColouredImageToBitMask;
using francor::processing::data::InputPort;
using francor::processing::data::OutputPort;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;
using francor::vision::Image;
using francor::vision::ColourSpace;
using francor::base::LineSegmentVector;
using francor::processing::NoDataType;

class FindLinesPipeline : public ProcessingPipeline<NoDataType, ColouredImageToBitMask, ExportClusteredPointsFromBitMask, DetectLineSegments>
{
public:
  FindLinesPipeline() : ProcessingPipeline<NoDataType, ColouredImageToBitMask, ExportClusteredPointsFromBitMask, DetectLineSegments>("find lines", 1, 1) { }

private:
  bool configureStages() final
  {
    bool ret = true;

    ret &= std::get<1>(_stages).input("bit mask").connect(std::get<0>(_stages).output("bit mask"));
    ret &= std::get<2>(_stages).input("clustered 2d points").connect(std::get<1>(_stages).output("clustered 2d points"));

    ret &= std::get<0>(_stages).input("coloured image").connect(this->input("coloured image"));
    ret &= std::get<2>(_stages).output("2d line segments").connect(this->output("line segments"));

    return ret;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<Image>(0, "coloured image");
    this->initializeOutputPort<LineSegmentVector>(0, "line segments");

    return true;
  }
};

FindLinesPipeline pipeline;
Image inputImage;

bool initialize(void)
{
  // colour range image filter

  pipeline.initialize();
  pipeline.input("coloured image").assign(&inputImage);

  return true;
}

int main(int argc, char** argv)
{
  francor::base::setLogLevel(francor::base::LogLevel::DEBUG);

  if (!initialize())
  {
    std::cout << "error occurred during initialization" << std::endl;
    return 1;
  }

  inputImage = std::move(francor::vision::loadImageFromFile("/home/knueppl/repos/francor/libfrancor/francor_vision/test/testdata/two-blue-lines.png",
                                                            ColourSpace::BGR));

  if (!pipeline.process())
  {
    std::cout << "error occurred during processing of pipeline" << std::endl;
    return 2;
  }

  for (const auto& segment : pipeline.output("line segments").data<LineSegmentVector>())
    std::cout << "found line segment: " << segment << std::endl;

  return 0;
}