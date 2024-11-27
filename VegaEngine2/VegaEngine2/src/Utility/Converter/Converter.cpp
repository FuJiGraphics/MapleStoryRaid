#include "pch.h"

namespace fz {

	MouseButtonType Converter::SfmlToFzType(sf::Mouse::Button button)
    {
        switch (button)
        {
            case sf::Mouse::Left:
                return MouseButtonType::Left;
            case sf::Mouse::Right:
                return MouseButtonType::Right;
            case sf::Mouse::Middle:
                return MouseButtonType::Middle;
            case sf::Mouse::XButton1:
                return MouseButtonType::XButton1;
            case sf::Mouse::XButton2:
                return MouseButtonType::XButton2;
            default:
                FZLOG_ASSERT(false, "타입 변환을 할 수 없습니다. 알 수 없는 타입입니다.");
                break;
        }

        return fz::MouseButtonType();
    }

	std::string Converter::ToString(Origins origin)
	{
		switch (origin)
		{
			case Origins::Custom:   return "Custom";
			case Origins::TL:       return "TL";
			case Origins::TC:       return "TC";
			case Origins::TR:       return "TR";
			case Origins::ML:       return "ML";
			case Origins::MC:       return "MC";
			case Origins::MR:       return "MR";
			case Origins::BL:       return "BL";
			case Origins::BC:       return "BC";
			case Origins::BR:       return "BR";
		}
		return "Unknown";
	}

	Origins Converter::StringToOrigins(const std::string& str)
	{
		if (str == "Custom")
			return Origins::Custom;
		else if (str == "TL")
			return Origins::TL;
		else if (str == "TC")
			return Origins::TC;
		else if (str == "TR")
			return Origins::TR;
		else if (str == "ML")
			return Origins::ML;
		else if (str == "MC")
			return Origins::MC;
		else if (str == "MR")
			return Origins::MR;
		else if (str == "BL")
			return Origins::BL;
		else if (str == "BC")
			return Origins::BC;
		else if (str == "BR")
			return Origins::BR;
		return Origins::Custom;
	}

} // namespace fz