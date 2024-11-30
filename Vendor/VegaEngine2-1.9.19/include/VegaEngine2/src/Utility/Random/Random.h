#pragma once

#include <SFML/Graphics.hpp>

namespace fz {

	/*
	* @brief 개별적인 기능을 담당하는 static 함수들을 모아놓은 클래스
	* @details
	* - 모든 멤버들은 static 선언, 정의가 되어있습니다.
	* - 모든 값은 임시 값으로 반환됩니다.
	* - 인자인 모든 데이터는 변경되지 않습니다.
	*/
	class Random_internal final
	{
	public:
		static Random_internal& GetInstance();	/*< Random의 인스턴스를 리턴합니다. */

		/*
		* @brief 랜덤 값을 리턴합니다.
		* @param[in] int start: 범위 시작 값
		* @param[in] int end: 범위 끝 값
		* @return int: 범위안의 값을 균일 분포로 랜덤 값을 리턴합니다.
		*/
		int operator()(int min, int max) const;
		/*
		* @brief 랜덤 값을 리턴합니다.
		* @param[in] float start: 범위 시작 값
		* @param[in] float end: 범위 끝 값
		* @return float: 범위안의 값을 균일 분포로 랜덤 값을 리턴합니다.
		*/
		float operator()(float min, float max) const;

		/*
		* @brief 랜덤 값(boolean)을 리턴합니다.
		* @return bool: true 또는 false를 리턴합니다.
		*/
		bool Boolean() const;

		/*
		* @brief 360도 범위안의 랜덤한 위치(0.0f ~ 1.0f)를 반환합니다.
		* @return sf::Vector2f: 360도 범위 안의 랜덤 위치(0.0f ~ 1.0f)
		*/
		sf::Vector2f InUnitCircle() const;
		/*
		* @brief 360도 범위안의 랜덤한 위치(0.0f ~ 1.0f)를 반환합니다.
		* @return sf::Vector2f: 360도 범위 안의 랜덤 위치(0.0f ~ 1.0f)
		*/
		sf::Vector2f InUnitCircle();

		/*
		* @brief min 이상 max 이하의 base 벡터 기준의 랜덤한 방향 벡터를 반환합니다.
		* @param[in] float min: 범위 시작 값
		* @param[in] float max: 범위 끝 값
		* @param[base] sf::Vector2f base: 기준이 되는 방향 벡터 (기본 값 x축)
		* @return min 이상 max 이하의 base 벡터 기준의 랜덤한 방향 벡터
		*/
		sf::Vector2f Direction(float angleMin, float angleMax, const sf::Vector2f& base = sf::Vector2f(1.0f, 0.0f));

		std::string GetUUID() const;
	};

	/*< Random_internal 접근 인스턴스 */
	static Random_internal& Random = Random_internal::GetInstance();

} // namespace fz
