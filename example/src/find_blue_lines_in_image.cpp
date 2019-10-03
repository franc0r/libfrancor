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

class FindLinesPipeline : public ProcessingPipeline<>
{
public:
  FindLinesPipeline() : ProcessingPipeline<>("find lines", 1, 1) { }

private:
  bool configureStages() final
  {
    auto colour_range   = std::make_unique<ColouredImageToBitMask>();
    auto extract_points = std::make_unique<ExportClusteredPointsFromBitMask>();
    auto detect_lines   = std::make_unique<DetectLineSegments>(100, 10, 3.0);

    bool ret = true;

    ret &= colour_range->initialize();
    ret &= extract_points->initialize();
    ret &= detect_lines->initialize();

    ret &= extract_points->input("bit mask").connect(colour_range->output("bit mask"));
    ret &= detect_lines->input("clustered 2d points").connect(extract_points->output("clustered 2d points"));

    ret &= colour_range->input("coloured image").connect(this->input("coloured image"));
    ret &= detect_lines->output("2d line segments").connect(this->output("line segments"));

    ret &= this->addStage(std::move(colour_range));
    ret &= this->addStage(std::move(extract_points));
    ret &= this->addStage(std::move(detect_lines));

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