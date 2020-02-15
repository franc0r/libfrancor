This library is only for experimental usage and is far away from release. The main goal of this piece of software
is to provide data processing and robot control components that make the life easier.

# Introduction

This library is a framework independent robotic library containing basic algorithm like RANSAC, ICP, etc. Also basic data structure are provided that allows easy and safety handling. For example it exists an normalized angle class that keeps the angle in the first interval.

The main goal of this library is to provides data processing pipelines that are aligned to industrial safety norms regarding autonomous driving. Usually it is required to check pre- and postconditions of data structure when they are modified. And to forward diagnostic data automatically of this data processing pipelines is targeted, too.

# Modules

## Algorithm

## Data Processing Pipeline

The data processing pipeline is a template class that provides techniques to handle a chain of processing stages. Each stage contains data ports that can be connected to others. The data port connection checks if the data type is correct.

The idea to uses pipelines is to have finish implemented operations that work on data structures (or also called model type). The pipeline do no contain a data structure only parameters and classes that are needed to process the data.

## Image Module

The image module wraps more or less opencv in that way the handling is more data type safe and user friendly. The new move simantic is used by this class, that means it exists a clear behavior visable to the user. Actually we tried to move the magic of the cv::Mat to the public. The image class provides a method to return a cv::Mat without any copying of data, so it can still be used as previously known from opencv.

# Installing