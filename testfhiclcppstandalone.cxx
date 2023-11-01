#include "fhiclcpp/ParameterSet.h"

int main(int argc, char const *argv[]) {
  std::unique_ptr<cet::filepath_maker> fm =
      std::make_unique<cet::filepath_maker>();

  fhicl::ParameterSet in_ps = fhicl::ParameterSet::make(argv[1], *fm);
  std::cout << in_ps.to_indented_string() << std::endl;
}