#pragma once
#include <juce_core/juce_core.h>
#include "Parameters.h"

namespace parametric_eq {
class JsonSerializer {
public:
  static void serialize(const Parameters&, juce::OutputStream&);

  /** @return Error message on failure; empty string otherwise.
   *           In case of error, no parameters are updated. */
  static juce::Result deserialize(juce::InputStream&, Parameters&);
};
}  // namespace parametric_eq
