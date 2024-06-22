module;

#define BACKWARD_HAS_DWARF 1
#include <backward.hpp>

export module Helium.Exceptions.TerminateHandler;

namespace helium::exceptions
{
backward::SignalHandling sh;
}