
namespace Components::lobster::Functions
{
	static auto PM_Weapon = reinterpret_cast<void(*)(Game::pmove_s * pm, Game::pml_t * pml)>(0x44C380);
	static auto PM_LadderMove = reinterpret_cast<void(*)(Game::pmove_s * pm, Game::pml_t * pml)>(0x573FC0);
	static auto PmoveSingle = reinterpret_cast<void(*)(Game::pmove_s * pm)>(0x5743E0);
	static auto Pmove = reinterpret_cast<void(*)(Game::pmove_s * pm)>(0x4CFEE0);
	//0x4CFEE0
}
