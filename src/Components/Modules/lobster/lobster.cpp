#include "lobster.hpp"
#include "Hooks.hpp"

namespace Components::lobster
{
	lobster::lobster()
	{
		Loader::Register(new Hooks());
	}
}
