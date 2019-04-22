#include "francor_processing/data_processing_pipeline.h"
#include "francor_vision/io.h"

using francor::processing::DataProcssingPipeline;
using francor::processing::DetectLines;
using francor::processing::ExportClusteredPointsFromBitMask;
using francor::processing::ColouredImageToBitMask;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;
using francor::vision::Image;
using francor::vision::ColourSpace;
using francor::base::LineVector;

DataProcssingPipeline pipeline;
Image inputImage;
SourcePort source(SourcePort::create<Image>("coloured image", &inputImage));
DestinationPort destination(DestinationPort::create<LineVector>("lines"));

bool initialize(void)
{
  // colour range image filter
  auto colourRange = std::make_unique<ColouredImageToBitMask>();

  if (!pipeline.addStage(std::move(colourRange)))
  {
    return false;
  }

  // extract points from bit mask
  auto extractPoints = std::make_unique<ExportClusteredPointsFromBitMask>();

  if (!pipeline.addStage(std::move(extractPoints)))
  {
    return false;
  }
  
  // find lines in point set
  auto detectLines = std::make_unique<DetectLines>(100, 10, 5.0);

  if (!pipeline.addStage(std::move(detectLines)))
  {
    return false;
  }


  pipeline.initialize();

  if (!pipeline.connectInputWithOutput("export clustered points from bit mask", "bit mask", "coloured image to bit mask", "bit mask"))
  {
    return false;
  }
  if (!pipeline.connectInputWithOutput("detect lines", "clustered 2d points", "export clustered points from bit mask", "clustered 2d points"))
  {
    return false;
  }
  if (!pipeline.connectDataSourcePort(source, "coloured image to bit mask", "coloured image"))
  {
    return false;
  }
  if (!pipeline.connectDataDestinationPort(destination, "detect lines", "2d lines"))
  {
    return false;
  }

  return true;
}

int main(int argc, char** argv)
{
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

  for (const auto& line : destination.data<LineVector>())
    std::cout << "found line: m = " << line.m() << ", t = " << line.t() << std::endl;

  return 0;
}