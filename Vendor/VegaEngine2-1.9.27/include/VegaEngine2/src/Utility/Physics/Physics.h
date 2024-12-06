#pragma once
#include "Raycast.h"

namespace fz {

	class Physics_internal final
	{
	public:
		static Physics_internal& GetInstance();
		static void Raycast(const sf::Vector2f& origin, const sf::Vector2f& direction, RaycastHit& hitInfo, float maxDistance, int layerMask);
	};

	/*< Random_internal ���� �ν��Ͻ� */
	static Physics_internal& Physics = Physics_internal::GetInstance();

} // namespace fz
