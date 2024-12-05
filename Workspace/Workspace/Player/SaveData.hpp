
namespace fz {

	class SaveData
	{
	public:
		SaveData() = default;
		~SaveData() = default;

		inline static bool ChangedScene = false;
		inline static sf::Vector2f Position = { 0.0f, 0.0f };
		inline static Stat Status;

		static void Set(const StatComponent& stat)
		{
			auto& src = stat.Stat;
			Status.HP = src.HP;
			Status.MP = src.MP;
			Status.AttackPower = src.AttackPower;
			Status.PhysicalDefense = src.PhysicalDefense;
			Status.MagicPower = src.MagicPower;
			Status.MagicDefense = src.MagicDefense;
			Status.Accuracy = src.Accuracy;
			Status.Evasion = src.Evasion;
			Status.Dexterity = src.Dexterity;
			Status.MoveSpeed = src.MoveSpeed;
			Status.JumpPower = src.JumpPower;

			Status.NumOfStr = src.NumOfStr;
			Status.NumOfDex = src.NumOfDex;
			Status.NumOfInt = src.NumOfInt;
			Status.NumOfLuk = src.NumOfLuk;
		}
		static void Get(StatComponent& stat)
		{
			auto& dst = stat.Stat;
			dst.HP = Status.HP;
			dst.MP = Status.MP;
			dst.AttackPower = Status.AttackPower;
			dst.PhysicalDefense = Status.PhysicalDefense;
			dst.MagicPower = Status.MagicPower;
			dst.MagicDefense = Status.MagicDefense;
			dst.Accuracy = Status.Accuracy;
			dst.Evasion = Status.Evasion;
			dst.Dexterity = Status.Dexterity;
			dst.MoveSpeed = Status.MoveSpeed;
			dst.JumpPower = Status.JumpPower;

			dst.NumOfStr = Status.NumOfStr;
			dst.NumOfDex = Status.NumOfDex;
			dst.NumOfInt = Status.NumOfInt;
			dst.NumOfLuk = Status.NumOfLuk;
		}
	};

}