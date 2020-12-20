/**
 * Unit test for the class ParameterBase and Parameter.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#include <gtest/gtest.h>

#include "francor_base/parameter.h"
#include "francor_base/parameter_constrain.h"

using francor::base::Parameter;
using francor::base::ConstrainMinimumValue;
using francor::base::ConstrainMaximumValue;
using francor::base::ParameterAdapterInterface;

/**
 * \brief Provides value strings for the parameter classes.
 */
class ParameterServer : public ParameterAdapterInterface
{
public:
  ParameterServer(const std::string& int_value, const std::string& double_value, const std::string& string_value)
    : _int_value(int_value),
      _double_value(double_value),
      _string_value(string_value) { }

  std::string getValueString(const std::string& parameter_name) const final
  {
    if ("int" == parameter_name) {
      return _int_value;
    }
    else if ("double" == parameter_name) {
      return _double_value;
    }
    else if ("string" == parameter_name) {
      return _string_value;
    }
    else {
      throw std::invalid_argument("Parameter name is unkown.");
    }
  }

private:
  const std::string _int_value;
  const std::string _double_value;
  const std::string _string_value;
};


/**
 * Try to instanciate a parameter object and check if initial value are set correctly.
 */
TEST(Parameter, Instanciate)
{
  const auto name = "int";
  const auto description = "a int value";
  const int default_value = 4711;

  // test first constructor without paramter adapter interface
  {
    Parameter<int> parameter_int(name, default_value, description);

    EXPECT_EQ(name, parameter_int.name());
    EXPECT_EQ(description, parameter_int.description());
    EXPECT_EQ(default_value, parameter_int.value());
  }

  // test second constructor with parameter adpater interface
  {
    auto parameter_server = std::make_shared<ParameterServer>("4711", "22.0", "33");
    Parameter<int> parameter_int(name, default_value, description, parameter_server);

    EXPECT_EQ(name, parameter_int.name());
    EXPECT_EQ(description, parameter_int.description());
    EXPECT_EQ(default_value, parameter_int.value());
  }
}

/**
 * Tries to read paramter value from apdater interface.
 */
TEST(Parameter, UpdateValue)
{
  Parameter<int> parameter_int("int", 0, "a int value");
  Parameter<double> parameter_double("double", 0.0, "a double value");
  Parameter<std::string> parameter_string("string", "", "a string value");
  auto parameter_server = std::make_shared<ParameterServer>("11", "22.0", "33");

  EXPECT_NO_THROW(parameter_int.updateValue(parameter_server));
  EXPECT_EQ(11, parameter_int.value());

  EXPECT_NO_THROW(parameter_double.updateValue(parameter_server));
  EXPECT_EQ(22.0, parameter_double.value());

  EXPECT_NO_THROW(parameter_string.updateValue(parameter_server));
  EXPECT_EQ("33", parameter_string.value());
}

/**
 * Check if invalid value string is detected.
 */
TEST(Parameter, UpdateInvalidValue)
{
  Parameter<int> parameter_int("int", 11, "a int value");
  Parameter<double> parameter_double("double", 22.0, "a double value");
  Parameter<std::string> parameter_string("string", "33", "a string value");
  auto parameter_server = std::make_shared<ParameterServer>("aa", "bb", "");

  EXPECT_ANY_THROW(parameter_int.updateValue(parameter_server));
  EXPECT_EQ(11, parameter_int.value());

  EXPECT_ANY_THROW(parameter_double.updateValue(parameter_server));
  EXPECT_EQ(22.0, parameter_double.value());

  EXPECT_ANY_THROW(parameter_string.updateValue(parameter_server));
  EXPECT_EQ("33", parameter_string.value());
}

/**
 * Check if the maximum and minimum value constrains detect out of range parameter values.
 */
TEST(ParameterConstrain, ValidValue)
{
  Parameter<int, ConstrainMaximumValue<int>, ConstrainMinimumValue<int>> parameter_int("int", 0, "a int value", {11}, {11});
  auto parameter_server = std::make_shared<ParameterServer>("11", "22.0", "33");

  parameter_int.updateValue(parameter_server);

  EXPECT_EQ(11, parameter_int.value());

  parameter_server = std::make_shared<ParameterServer>("12", "22.0", "33");
  EXPECT_ANY_THROW(parameter_int.updateValue(parameter_server));
  EXPECT_EQ(11, parameter_int.value());

  parameter_server = std::make_shared<ParameterServer>("10", "22.0", "33");
  EXPECT_ANY_THROW(parameter_int.updateValue(parameter_server));
  EXPECT_EQ(11, parameter_int.value());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}