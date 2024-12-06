#pragma once
#include "Raycast.h"

namespace fz {

	class Physics_internal final
	{
	public:
		static Physics_internal& GetInstance();
		static void Raycast(const sf::Vector2f& origin, float angle, RaycastHit& hitInfo, float maxDistance);
		static void Raycast(const sf::Vector2f& origin, const sf::Vector2f& direction, RaycastHit& hitInfo, float maxDistance);
		static void SetRaycastDebug(bool enabled);

	private:
		inline static bool s_DebugBox = false;
	};

	/*< Random_internal 접근 인스턴스 */
	static Physics_internal& Physics = Physics_internal::GetInstance();

} // namespace fz

