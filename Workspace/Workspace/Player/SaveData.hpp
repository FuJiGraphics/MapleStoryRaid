
namespace fz {

	class SaveData
	{
	public:
		SaveData() = default;
		~SaveData() = default;

		inline static bool ChangedScene = false;
		inline static sf::Vector2f Position = { 0.0f, 0.0f };
		inline static int HP = 100;
		inline static int MP = 100;
		inline static int AD = 0;
		inline static int AP = 0;

		static void Set(const StatComponent& stat)
		{
			HP = stat.Stat.HP;
			MP = stat.Stat.MP;
			AD = stat.Stat.AD;
			AP = stat.Stat.AP;
		}
		static void Get(StatComponent& stat)
		{
			stat.Stat.HP = HP;
			stat.Stat.MP = MP;
			stat.Stat.AD = AD;
			stat.Stat.AP = AP;
		}
	};

}