#define _USE_MATH_DEFINES
#include <Novice.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const char kWindowTitle[] = "GC1A_05_ゴ_ウ";
#pragma region 代码折叠用代码
//==================
//代码折叠用代码
#pragma region
#pragma endregion
//==================
#pragma endregion
#pragma region 全局 グローバル

//复活时间 
const int respawnTimerValue = 300;
//出现时间
const int enemyAppearTimerValue = 30;
//游戏时长
const float timerValue = 66.0f;
#pragma region 構造体

//===================================================
// 構造体
//===================================================
#pragma region 2次元ベクトル

typedef struct Vector2 {//2次元向量
	float x;//x坐标
	float y;//y坐标
}Vector2;
#pragma endregion
#pragma region プレイヤー
typedef struct Player {//玩家控制角色
	Vector2 Position; //world position
	Vector2 moveVelocity;//移动速度
	Vector2 moveVelocityMax;//移动速度
	float width;//宽
	float height;//高
	int job;//职业
	int attribute;//属性
	float lifePoint;//生命值
	float lifePointMax;//生命值
	float attack;//攻击力
	float defense;//防御力
	int shootCoolTime;//发射间隔
	float velocityLength;//速度正规化时用的长度
	Vector2 normalizeVelocity;//正规化后的速度
	int isTurnRight;//是否向右
	int level;//等级
	int levelMax;//最大等级
	int experience;
	int experienceMax;
	int levelUPExperience[26];//升级用经验值表
	int isInvincible;
	int invincibleTimer;
}Player;
#pragma endregion
#pragma region 敵
typedef struct Enemy {//敌人
	Vector2 Position; //world position
	Vector2 moveVelocity;//移动速度
	float width;//宽
	float height;//高
	int attribute;//属性
	float lifePoint;//生命值
	float lifePointMax;//生命值
	float attack;//攻击力
	float defense;//防御力
	int isEnemyAlive;//生存flag
	int isEnemyRealAlive;//生存flag
	int isMove;//移动flag
	Vector2 vector;//正规化时所用参量——向量
	Vector2 vectorNew;//正规化时所用参量——新向量
	float length;//正规化时所用参量——长度
	int respawnTimer;//复活时间
	int enemyAppearTimer;//出现时间
	int attackCoolTime;//攻击冷却
	int isEnemyImmunity;//免疫伤害
	int isStop;//停顿效果
	int isStopTimer;//停顿时间
}Enemy;
#pragma endregion
#pragma region 弾
typedef struct Bullet {
	Vector2 Position;//位置
	float width;//宽
	float height;//高
	float velocity;//速度
	int isShoot;//射击flag
	int isExist;//存在flag
	float existTime;//存在时间
	Vector2 velocityVector;//正规化用参数
	Vector2 velocityVectorNew;//正规化用参数
	float length;//正规化用参数
}Bullet;

typedef struct BulletBoss {
	Vector2 Position;//位置
	float width;//宽
	float height;//高
	float velocity;//速度
	int isShoot;//射击flag
	int isExist;//存在flag
}BulletBoss;
#pragma endregion
#pragma region アイテム
typedef struct ITEM {
	int name;
	Vector2 speed;//速度
	float lifePointMax;//生命值
	float attack;//攻击力
	float defense;//防御力
	int cost;//花费

}ITEM;
#pragma endregion
#pragma endregion
#pragma region 関数
//===================================================
// 関数
//===================================================

/// <summary>
/// 伤害计算函数 ダメージ計算
/// </summary>
/// <param name="receivedDamage">受到伤害 受けるダメージ</param>
/// <param name="selfDefense">自身防御力 自分の防御力</param>
/// <returns>实际伤害 実際のダメージ</returns>
float DamageCalculation(float receivedDamage, float selfDefense) {
	float realDamage;
	realDamage = receivedDamage * (1 - (selfDefense / (selfDefense + 20)));
	return realDamage;
}

/// <summary>
/// 函数:递归法求数组中最小值，并记录位置
/// </summary>
/// <param name="arr">保存距离的数组</param>
/// <param name="size">数组的长度</param>
/// <param name="minIndex">最小值的地址</param>
/// <returns>数组中的最小值</returns>
float findMin(float arr[], int size, int* minIndex) {

	// 递归基本情况：如果数组只有一个元素，则将其作为最小值返回，并将位置设置为0（因为数组中只有一个元素）。
	if (size == 1) {
		*minIndex = 0;  // 最小值的位置为0（即数组的第一个元素）
		return arr[0];
	}

	// 递归情况：寻找剩余数组（排除第一个元素）的最小值。
	int subArrayMinIndex;
	float minRest = findMin(arr + 1, size - 1, &subArrayMinIndex);

	// 将第一个元素与剩余数组的最小值进行比较，并根据需要更新最小值的位置。
	if (arr[0] < minRest) {
		*minIndex = 0; // 更新最小值的位置为当前元素的位置
		return arr[0];  // 返回第一个元素作为新的最小值
	}
	else {
		*minIndex = subArrayMinIndex + 1; // 根据子数组的最小值位置更新最小值的位置
		return minRest; // 返回剩余数组的最小值
	}
}

float minOfFive(float a, float b, float c, float d, float e) {
	float min = a; // 假设第一个数字是最小值

	if (b < min) {
		min = b;
	}
	if (c < min) {
		min = c;
	}
	if (d < min) {
		min = d;
	}
	if (e < min) {
		min = e;
	}

	return min;
}


#pragma endregion
#pragma endregion
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//随机数种子
	srand((unsigned)time(NULL));
#pragma region 金钱 お金
	//图像加载  画像
	int moneyNumberTexture[10];
	moneyNumberTexture[0] = Novice::LoadTexture("./Resources/Image/Number/Money/0.png");
	moneyNumberTexture[1] = Novice::LoadTexture("./Resources/Image/Number/Money/1.png");
	moneyNumberTexture[2] = Novice::LoadTexture("./Resources/Image/Number/Money/2.png");
	moneyNumberTexture[3] = Novice::LoadTexture("./Resources/Image/Number/Money/3.png");
	moneyNumberTexture[4] = Novice::LoadTexture("./Resources/Image/Number/Money/4.png");
	moneyNumberTexture[5] = Novice::LoadTexture("./Resources/Image/Number/Money/5.png");
	moneyNumberTexture[6] = Novice::LoadTexture("./Resources/Image/Number/Money/6.png");
	moneyNumberTexture[7] = Novice::LoadTexture("./Resources/Image/Number/Money/7.png");
	moneyNumberTexture[8] = Novice::LoadTexture("./Resources/Image/Number/Money/8.png");
	moneyNumberTexture[9] = Novice::LoadTexture("./Resources/Image/Number/Money/9.png");
	int moneyTexture = Novice::LoadTexture("./Resources/Image/Number/Money/money.png");
	//金钱变量  変数
	int moneyNumber = 0;
	//数字位数  桁数
	int eachMoneyNumber[4] = {};
#pragma endregion
#pragma region 游戏计时器 ゲームタイマー
	//图像加载
	int timerNumberTexture[10];
	timerNumberTexture[0] = Novice::LoadTexture("./Resources/Image/Number/Time/0.png");
	timerNumberTexture[1] = Novice::LoadTexture("./Resources/Image/Number/Time/1.png");
	timerNumberTexture[2] = Novice::LoadTexture("./Resources/Image/Number/Time/2.png");
	timerNumberTexture[3] = Novice::LoadTexture("./Resources/Image/Number/Time/3.png");
	timerNumberTexture[4] = Novice::LoadTexture("./Resources/Image/Number/Time/4.png");
	timerNumberTexture[5] = Novice::LoadTexture("./Resources/Image/Number/Time/5.png");
	timerNumberTexture[6] = Novice::LoadTexture("./Resources/Image/Number/Time/6.png");
	timerNumberTexture[7] = Novice::LoadTexture("./Resources/Image/Number/Time/7.png");
	timerNumberTexture[8] = Novice::LoadTexture("./Resources/Image/Number/Time/8.png");
	timerNumberTexture[9] = Novice::LoadTexture("./Resources/Image/Number/Time/9.png");

	//计时器变量
	float gameTimer = timerValue;
	//数字位数
	int eachTimerNumber[2] = {};
#pragma endregion
#pragma region シーンの切り替えの宣言
	//=============================================
	// シーンの切り替え
	//=============================================
#pragma region 画像

	//画像

	//标题
	int TEXTURE_START_BACKGROUND_TITLE = Novice::LoadTexture("./Resources/Image/Background/Start_background/Background_title.png");
	//读取中
	int TEXTURE_START_BACKGROUND_LOADING = Novice::LoadTexture("./Resources/Image/Background/Start_background/Background_loading.png");
	//主界面选项
	int TEXTURE_START_BACKGROUND_SELECTION[7] = {
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_start_1.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_start_2.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_character_1.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_character_2.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_option_1.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_option_2.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Select/Select_selection.png")
	};
	//角色介绍
	int TEXTURE__START_BACKGROUND_CHARACTER[3] = {
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Character_introduction/character_introduction_warrior.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Character_introduction/character_introduction_mage.png"),
		Novice::LoadTexture("./Resources/Image/Background/Start_background/Character_introduction/character_introduction_priest.png")
	};
	//选项（操作方法）
	int TEXTURE__START_BACKGROUND_OPTION = Novice::LoadTexture("./Resources/Image/Background/Start_background/Option/option.png");

	//结局和死亡图像
	int TEXTURE_HE = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_he.png");
	int TEXTURE_BE = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_be.png");
	int TEXTURE_DEATH = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_death.png");
#pragma endregion
	//シーンの切り替えのタイマー
	int SCENE_CHANGE_TIMER = 60;

#pragma region シーン enum
	//シーンの名前
	enum Scene {
		NOTHING,
		START,//开始
		CHARACTER,//角色介绍
		OPTION,//操作方法
		LOADING,//过场
		STAGE_PASSED_REWARD,//奖励选择
		SHOP,//商店
		STAGE1,//关卡1
		STAGE2,//关卡2
		BOSS,//关卡boss
		END1,//结局1 bad
		END2,//结局2 happy
		DEATH//死亡
	};
	Scene scene = START;
	Scene preScene = NOTHING;
	Scene lastStage = NOTHING;
	//STARTシーンの選択肢
	enum START_SELECT {
		GAMESTART_SELECT,//主界面选项——开始游戏
		CHARACTER_SELECT,//主界面选项——角色介绍
		OPTION_SELECT//主界面选项——操作方法
	};
	START_SELECT StartSelect = GAMESTART_SELECT;

	//CHARACTERの選択肢
	enum CHARACTER_INTRODUCTION {
		WARRIOR_INTRODUCTION,//角色介绍——战士
		MAGE_INTRODUCTION,//角色介绍——法师
		PRIEST_INTRODUCTION//角色介绍——牧师
	};
	CHARACTER_INTRODUCTION CharacterIntroduction = WARRIOR_INTRODUCTION;

	//StagePassedのReward選択肢
	enum REWARD_SELECTION {
		REWARD_1,
		REWARD_2,
		REWARD_3
	};
	REWARD_SELECTION RewardSelection = REWARD_1;

	//商店选项
	enum SHOP_SELECTION {
		ITEM_1,
		ITEM_2,
		ITEM_3,
	};
	SHOP_SELECTION ShopSelection = ITEM_1;
#pragma endregion
#pragma endregion
#pragma region 游戏中 ゲーム中
	//=============================================
	// ゲーム中
	//=============================================
#pragma region 背景

	//背景
	int TEXTURE_GAME_BACKGROUND_GAMING1 = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_game1.png");
	int TEXTURE_GAME_BACKGROUND_GAMING2 = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_game2.png");
	int TEXTURE_GAME_BACKGROUND_GAMING3 = Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_game3.png");
	int TEXTURE_GAME_BACKGROUND_BOSS[4] = {
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_boss1.png"),
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_boss2.png"),
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_boss3.png"),
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_boss4.png")
	};
	//字幕
	int TEXTURE_GAME_BANNER[3] = {
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_banner1.png"),
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_banner2.png"),
		Novice::LoadTexture("./Resources/Image/Background/Game_background/Background_banner3.png")
	};
#pragma endregion
#pragma region 音声
	//加载音声
	int SOUND_CONFIRM1 = Novice::LoadAudio("./Resources/Sound/confirm.wav");//确认1
	int SOUND_CONFIRM2 = Novice::LoadAudio("./Resources/Sound/confirm_2.wav");//确认2
	int SOUND_HIT = Novice::LoadAudio("./Resources/Sound/hit.wav");//被打
	int SOUND_SELECT = Novice::LoadAudio("./Resources/Sound/select.wav");//选择
	int SOUND_BGM_STAGE1 = Novice::LoadAudio("./Resources/Sound/stage1_bgm.wav");//关卡1bgm
	int SOUND_BGM_STAGE2 = Novice::LoadAudio("./Resources/Sound/stage2_bgm.wav");//关卡2bgm
	int SOUND_BGM_STAGEBOSS = Novice::LoadAudio("./Resources/Sound/stageboss_bgm.wav");//关卡bossbgm
	int SOUND_BGM_STAGESTART = Novice::LoadAudio("./Resources/Sound/start_bgm.wav");//关卡bossbgm
	//控制音声
	int voiceHandle_Confirm1 = -1;
	int voiceHandle_Confirm2 = -1;
	int voiceHandle_Hit = -1;
	int voiceHandle_Select = -1;
	int voiceHandle_Stage1 = -1;
	int voiceHandle_Stage2 = -1;
	int voiceHandle_StageBoss = -1;
	int voiceHandle_StageStart = -1;

#pragma endregion
#pragma endregion
#pragma region 参数 各種の変数 引数
#pragma region 过关用参数 パス用
	bool isStagePassed = false;//是否过关
	bool isPaused = false;//是否暂停
#pragma endregion
#pragma region 距离计算用参数 距離計算用
	//敌人和自机距离 保存的数组
	float distance1[50];
	for (int i = 0; i < 50; i++) {
		distance1[i] = 1300;
	}
	float distance2[50];
	for (int i = 0; i < 50; i++) {
		distance2[i] = 1300;
	}
	float distance3[50];
	for (int i = 0; i < 50; i++) {
		distance3[i] = 1300;
	}
	float distance4[50];
	for (int i = 0; i < 50; i++) {
		distance4[i] = 1300;
	}
	//敌人和自机距离最小值的脚标
	int dMin1 = 0;
	int dMin2 = 0;
	int dMin3 = 0;
	int dMin4 = 0;
	//数组长度
	int size1 = 0;
	int size2 = 0;
	int size3 = 0;
	int size4 = 0;
	//数组最小值
	float dinstanceMin1 = 1300.0f;
	float dinstanceMin2 = 1300.0f;
	float dinstanceMin3 = 1300.0f;
	float dinstanceMin4 = 1300.0f;
	float dinstanceMin5 = 1300.0f;
	float dinstanceMinOfMin = 1300.0f;
#pragma endregion
#pragma region 自机参数 プレイヤーの変数 引数
	//=============================================
	// 自机参数
	//=============================================
#pragma region 画像
	//画像

	//战士
	int TEXTURE_PLAYER_WARRIOR[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Warrior/Warrior_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Warrior/Warrior_right.png")
	};
	//战士武器
	int TEXTURE_WEAPON_WARRIOR[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Warrior_axe_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Warrior_axe_right.png")
	};
	//法师
	int TEXTURE_PLAYER_MAGE[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Mage/Mage_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Mage/Mage_right.png")
	};
	//法师武器
	int TEXTURE_WEAPON_MAGE[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Mage_staff_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Mage_staff_right.png")
	};
	//牧师
	int TEXTURE_PLAYER_PRIEST[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Priest/Priest_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Priest/Priest_right.png")
	};
	//牧师武器
	int TEXTURE_WEAPON_PRIEST[2] = {
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Priest_cross_left.png"),
		Novice::LoadTexture("./Resources/Image/Character/Weapon/Priest_cross_right.png")
	};
	//生命条
	int TEXTURE_PLAYER_LIFEBAR = Novice::LoadTexture("./Resources/Image/Character/Player_lifebar.png");

#pragma endregion

#pragma region 人物
	//人物

	enum PLAYER_CHARACTER {//人物职业
		WARRIOR,//战士
		MAGE,   //法师
		PRIEST  //牧师
	};
	PLAYER_CHARACTER Player_character = WARRIOR;

	enum PLAYER_ATTRIBUTE {//人物属性
		PHYSICS,//物理伤害
		MAGIC,  //魔法伤害
		HOLY    //神圣伤害
	};
	PLAYER_ATTRIBUTE Player_attribute = PHYSICS;

	Player player;
	player.Position.x = 640.0f;
	player.Position.y = 500.0f;
	player.moveVelocity.x = 5;
	player.moveVelocity.y = 5;
	player.moveVelocityMax.x = 5;
	player.moveVelocityMax.y = 5;
	player.width = 64.0f;
	player.height = 64.0f;
	player.job = Player_character;
	player.attribute = Player_attribute;
	player.lifePoint = 50.0f;
	player.lifePointMax = 50.0f;
	player.attack = 20.0f;
	player.defense = 15.0f;
	player.shootCoolTime = 0;
	player.velocityLength = 0;
	player.normalizeVelocity.x = 0;
	player.normalizeVelocity.y = 0;
	player.isTurnRight = false;
	player.level = 0;
	player.levelMax = 25;
	player.experience = 0;
	player.experienceMax = 0;
	player.levelUPExperience[player.levelMax + 1];
	for (int i = 0; i < player.levelMax + 1; i++) {
		player.levelUPExperience[i] = 100 * i;
	}
	player.isInvincible = false;
	player.invincibleTimer = 30;
	int rollHealing = 0;
#pragma endregion
#pragma endregion
#pragma region 敵の変数 引数
	//=============================================
	// 敵参数
	//=============================================
#pragma region 画像

	//画像

	//哥布林
	int TEXTURE_ENEMY_GOBLIN = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_goblin.png");
	//幽灵
	int TEXTURE_ENEMY_GHOST = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_ghost.png");
	//恶魔
	int TEXTURE_ENEMY_DEMON = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_demon.png");
	//龙
	int TEXTURE_ENEMY_DRAGON = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_dragon.png");
	//boss
	int TEXTURE_ENEMY_BOSS = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_boss.png");
	//刷怪动画
	int TEXTURE_ENEMY_BORN = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_born.png");
	//死亡动画
	int TEXTURE_ENEMY_DEATH = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_death.png");
	//免疫特效
	int TEXTURE_ENEMY_IMMUNITY = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_mukou.png");
#pragma endregion

	//敌人属性
	enum ENEMY_ATTRIBUTE {//敌人属性
		GOBLIN_NORMAL,    //哥布林——通常
		GHOST_VOID,       //幽灵——虚无
		DEMON_EVIL,       //恶魔——邪恶
		DRAGON_FANTASTIC,  //龙——幻想
		BOSS_BOSS
	};
#pragma region goblin参数
	Enemy Goblin[50];
	for (int i = 0; i < 50; i++) {
		Goblin[i].Position.x = -100.0f;                //world position
		Goblin[i].Position.y = -100.0f;                //world position
		Goblin[i].moveVelocity.x = 2.5f;               //移动速度
		Goblin[i].moveVelocity.y = 2.5f;               //移动速度
		Goblin[i].width = 64;                          //宽
		Goblin[i].height = 64;                         //高
		Goblin[i].attribute = GOBLIN_NORMAL;           //属性
		Goblin[i].lifePoint = 40;                      //生命值
		Goblin[i].lifePointMax = 40;                   //生命值
		Goblin[i].attack = 3;                          //攻击力
		Goblin[i].defense = 0;                         //防御力
		Goblin[i].isEnemyAlive = false;                //生存flag 
		Goblin[i].isEnemyRealAlive = true;             //生存flag2
		Goblin[i].isMove = false;                      //移动flag
		Goblin[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
		Goblin[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
		Goblin[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
		Goblin[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
		Goblin[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
		Goblin[i].respawnTimer = respawnTimerValue;    //复活计时器
		Goblin[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
		Goblin[i].attackCoolTime = 60;//攻击计时器
		Goblin[i].isEnemyImmunity = false;
		Goblin[i].isStop = false;
		Goblin[i].isStopTimer = 10;
	}
#pragma endregion
#pragma region ghost参数
	Enemy Ghost[50];
	for (int i = 0; i < 50; i++) {
		Ghost[i].Position.x = -100.0f;                //world position
		Ghost[i].Position.y = -100.0f;                //world position
		Ghost[i].moveVelocity.x = 4.0f;               //移动速度
		Ghost[i].moveVelocity.y = 4.0f;               //移动速度
		Ghost[i].width = 64;
		Ghost[i].height = 64;
		Ghost[i].attribute = GHOST_VOID;              //属性
		Ghost[i].lifePoint = 25;                      //生命值
		Ghost[i].lifePointMax = 25;                   //生命值
		Ghost[i].attack = 5;                          //攻击力
		Ghost[i].defense = 1;                         //防御力
		Ghost[i].isEnemyAlive = false;                //生存flag 
		Ghost[i].isEnemyRealAlive = true;             //生存flag2
		Ghost[i].isMove = false;                      //移动flag
		Ghost[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
		Ghost[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
		Ghost[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
		Ghost[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
		Ghost[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
		Ghost[i].respawnTimer = respawnTimerValue;    //复活计时器
		Ghost[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
		Ghost[i].attackCoolTime = 60;
		Ghost[i].isEnemyImmunity = false;
		Ghost[i].isStop = false;
		Ghost[i].isStopTimer = 10;
	}
#pragma endregion
#pragma region demon参数
	Enemy Demon[50];
	for (int i = 0; i < 50; i++) {
		Demon[i].Position.x = -100.0f;                //world position
		Demon[i].Position.y = -100.0f;                //world position
		Demon[i].moveVelocity.x = 2.0f;               //移动速度
		Demon[i].moveVelocity.y = 2.0f;               //移动速度
		Demon[i].width = 64;
		Demon[i].height = 64;
		Demon[i].attribute = DEMON_EVIL;              //属性
		Demon[i].lifePoint = 60;                      //生命值
		Demon[i].lifePointMax = 60;                   //生命值
		Demon[i].attack = 6;                          //攻击力
		Demon[i].defense = 2;                         //防御力
		Demon[i].isEnemyAlive = false;                //生存flag 
		Demon[i].isEnemyRealAlive = true;             //生存flag2
		Demon[i].isMove = false;                      //移动flag
		Demon[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
		Demon[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
		Demon[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
		Demon[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
		Demon[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
		Demon[i].respawnTimer = respawnTimerValue;    //复活计时器
		Demon[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
		Demon[i].attackCoolTime = 60;
		Demon[i].isEnemyImmunity = false;
		Demon[i].isStop = false;
		Demon[i].isStopTimer = 10;
	}
#pragma endregion
#pragma region dragon参数
	Enemy Dragon[50];
	for (int i = 0; i < 50; i++) {
		Dragon[i].Position.x = -100.0f;                //world position
		Dragon[i].Position.y = -100.0f;                //world position
		Dragon[i].moveVelocity.x = 1.5f;               //移动速度
		Dragon[i].moveVelocity.y = 1.5f;               //移动速度
		Dragon[i].width = 64;
		Dragon[i].height = 64;
		Dragon[i].attribute = DRAGON_FANTASTIC;        //属性
		Dragon[i].lifePoint = 120;                     //生命值
		Dragon[i].lifePointMax = 120;                  //生命值
		Dragon[i].attack = 10;                         //攻击力
		Dragon[i].defense = 5;                         //防御力
		Dragon[i].isEnemyAlive = false;                //生存flag 
		Dragon[i].isEnemyRealAlive = true;             //生存flag2
		Dragon[i].isMove = false;                      //移动flag
		Dragon[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
		Dragon[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
		Dragon[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
		Dragon[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
		Dragon[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
		Dragon[i].respawnTimer = respawnTimerValue;    //复活计时器
		Dragon[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
		Dragon[i].attackCoolTime = 60;
		Dragon[i].isEnemyImmunity = false;
		Dragon[i].isStop = false;
		Dragon[i].isStopTimer = 10;
	}
#pragma endregion
#pragma endregion
#pragma region bossの変数 引数
	Enemy Boss;
	Boss.Position.x = -100.0f;                //world position
	Boss.Position.y = -100.0f;                //world position
	Boss.moveVelocity.x = 3.0f;               //移动速度
	Boss.moveVelocity.y = 3.0f;               //移动速度
	Boss.width = 128;                          //宽
	Boss.height = 128;                         //高
	Boss.attribute = BOSS_BOSS;           //属性
	Boss.lifePoint = 5000;                      //生命值
	Boss.lifePointMax = 5000;                   //生命值
	Boss.attack = 50;                          //攻击力
	Boss.defense = 30;                         //防御力
	Boss.isEnemyAlive = false;                //生存flag 
	Boss.isEnemyRealAlive = true;             //生存flag2
	Boss.isMove = false;                      //移动flag
	Boss.vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
	Boss.vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
	Boss.vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
	Boss.vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
	Boss.length = 0.0f;                       // 计算距离及正规化的时候用的参量
	Boss.respawnTimer = respawnTimerValue;    //复活计时器
	Boss.enemyAppearTimer = enemyAppearTimerValue;//出现计时器
	Boss.attackCoolTime = 0;//攻击计时器
	Boss.isEnemyImmunity = false;

#pragma endregion
#pragma region bossの弾の変数 引数
	//画像
	int TEXTURE_BOSS_BULLET = Novice::LoadTexture("./Resources/Image/Enemy/Enemy_boss_bullet.png");

	BulletBoss bulletBoss[32];
	for (int i = 0; i < 32; i++) {
		bulletBoss[i].Position.x = -128;
		bulletBoss[i].Position.y = -128;
		bulletBoss[i].width = 32;
		bulletBoss[i].height = 32;
		bulletBoss[i].velocity = 6;
		bulletBoss[i].isShoot = false;
		bulletBoss[i].isExist = false;
	}
	int reloadTimer = 600;
	float theta[32] = { 0 };
	for (int i = 0; i < 32; i++) {
		theta[i] = i * (1.0f / 16 * float(M_PI));
	}
#pragma endregion
#pragma region 弾のの変数 引数
#pragma region warrior子弹
	//画像
	int TEXTURE_WARRIOR_BULLET = Novice::LoadTexture("./Resources/Image/Character/Bullet/Warrior_bullet.png");
	//战士
	Bullet warriorBullet[50];
	for (int i = 0; i < 50; i++) {
		warriorBullet[i].Position.x = -200;
		warriorBullet[i].Position.y = -200;
		warriorBullet[i].width = 32;
		warriorBullet[i].height = 32;
		warriorBullet[i].velocity = 35;
		warriorBullet[i].isShoot = false;
		warriorBullet[i].isExist = false;
		warriorBullet[i].existTime = 0.0f;
		warriorBullet[i].velocityVector.x = 0;
		warriorBullet[i].velocityVector.y = 0;
		warriorBullet[i].velocityVectorNew.x = 0;
		warriorBullet[i].velocityVectorNew.y = 0;
		warriorBullet[i].length = 0;
	}
#pragma endregion
#pragma region mage子弹
	//画像
	int TEXTURE_MAGE_BULLET = Novice::LoadTexture("./Resources/Image/Character/Bullet/Mage_bullet.png");
	//法师
	Bullet mageBullet[50];
	for (int i = 0; i < 50; i++) {
		mageBullet[i].Position.x = -200;
		mageBullet[i].Position.y = -200;
		mageBullet[i].width = 32;
		mageBullet[i].height = 32;
		mageBullet[i].velocity = 35;
		mageBullet[i].isShoot = false;
		mageBullet[i].isExist = false;
		mageBullet[i].existTime = 0;
		mageBullet[i].velocityVector.x = 0;
		mageBullet[i].velocityVector.y = 0;
		mageBullet[i].velocityVectorNew.x = 0;
		mageBullet[i].velocityVectorNew.y = 0;
		mageBullet[i].length = 0;
	}
#pragma endregion
#pragma region priest子弹
	//画像
	int TEXTURE_PRIEST_BULLET = Novice::LoadTexture("./Resources/Image/Character/Bullet/Priest_bullet.png");
	//牧师
	Bullet priestBullet[50];
	for (int i = 0; i < 50; i++) {
		priestBullet[i].Position.x = -200;
		priestBullet[i].Position.y = -200;
		priestBullet[i].width = 32;
		priestBullet[i].height = 32;
		priestBullet[i].velocity = 35;
		priestBullet[i].isShoot = false;
		priestBullet[i].isExist = false;
		priestBullet[i].existTime = 0;
		priestBullet[i].velocityVector.x = 0;
		priestBullet[i].velocityVector.y = 0;
		priestBullet[i].velocityVectorNew.x = 0;
		priestBullet[i].velocityVectorNew.y = 0;
		priestBullet[i].length;
	}
#pragma endregion
#pragma endregion
#pragma region BUFFの変数 引数
	//背景图像
	int TEXTURE_BUFF_REWARD = Novice::LoadTexture("./Resources/Image/Background/Reward_background/Background_reward.png");

	//选择框
	int TEXTURE_SELECTION = Novice::LoadTexture("./Resources/Image/Item/select.png");

	//图像
	int TEXTURE_BUFF_ATK = Novice::LoadTexture("./Resources/Image/Buff/Buff_atk_new.png");
	int TEXTURE_BUFF_DEF = Novice::LoadTexture("./Resources/Image/Buff/Buff_def_new.png");
	int TEXTURE_BUFF_SPD = Novice::LoadTexture("./Resources/Image/Buff/Buff_spd_new.png");
	int TEXTURE_BUFF_LIF = Novice::LoadTexture("./Resources/Image/Buff/Buff_lif_new.png");
	int TEXTURE_BUFF_ON[4];

	bool isBuffRandom = false;

	enum BUFF {
		nothing_buff,
		speedBuff,
		lifeBuff,
		attackBuff,
		defenseBuff
	};
	BUFF buff[4] = { nothing_buff ,nothing_buff ,nothing_buff ,nothing_buff };
	BUFF buffMap[5] = {
		nothing_buff,
		speedBuff,
		lifeBuff,
		attackBuff,
		defenseBuff
	};
#pragma endregion
#pragma region アイテムの変数 引数
	//背景图像
	int TEXTURE_SHOP_BACKGROUND = Novice::LoadTexture("./Resources/Image/Background/Shop_background/Background_shop.png");
	//图像 
	int TEXTURE_ITEM_SPEEDBOOTS = Novice::LoadTexture("./Resources/Image/Item/Item_speedboots.png");
	int TEXTURE_ITEM_LIFENECKLACE = Novice::LoadTexture("./Resources/Image/Item/Item_lifenecklace.png");
	int TEXTURE_ITEM_POWERRING = Novice::LoadTexture("./Resources/Image/Item/Item_powerring.png");
	int TEXTURE_ITEM_BUCKLER = Novice::LoadTexture("./Resources/Image/Item/Item_Buckler.png");
	int TEXTURE_ITEM_HOLYEMBLEM = Novice::LoadTexture("./Resources/Image/Item/Item_HolyEmblem.png");
	int TEXTURE_ITEM_DSSWORD = Novice::LoadTexture("./Resources/Image/Item/Item_DragonSlayingSword.png");
	int TEXTURE_ITEM_ON[4];

	bool isItemRandom = false;
	bool isBought1 = false;
	bool isBought2 = false;
	bool isBought3 = false;
	bool isSuccess = false;


	enum ITEMLIST {
		nothing_item,
		SpeedBoots,
		LifeNecklace,
		PowerRing,
		Buckler,
		HolyEmblem,
		DragonSlayingSword
	};
	ITEMLIST Itemlist[4] = { nothing_item ,nothing_item ,nothing_item ,nothing_item };
	ITEMLIST ItemlistMap[7] = {
		nothing_item,
		SpeedBoots,
		LifeNecklace,
		PowerRing,
		Buckler,
		HolyEmblem,
		DragonSlayingSword
	};
	ITEM Item_Select[7];
	Item_Select[0] = {
		nothing_item,
		{0,0},
		0,
		0,
		0,
		0
	};
	Item_Select[1] = {
		SpeedBoots,
		{2,2},//速度
		0,//生命
		0,//攻击
		0,//防御
		20
	};
	Item_Select[2] = {
		LifeNecklace,
		{0,0},
		10,
		0,
		0,
		30
	};
	Item_Select[3] = {
		PowerRing,
		{0,0},
		0,
		5,
		0,
		35
	};
	Item_Select[4] = {
		Buckler,
		{0,0},
		0,
		0,
		5,
		35
	};
	Item_Select[5] = {
		HolyEmblem,
		{1,1},
		5,
		5,
		5,
		100
	};
	Item_Select[6] = {
		DragonSlayingSword,
		{0,0},
		0,
		100,
		0,
		500
	};
#pragma endregion
#pragma region マップチップ参数
	int size = 64;
	int map[12][20] = {
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0},
		{0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
		{0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,1},
		{0,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1},
		{0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	};
	Vector2 mapPosition = {
		0.0f,
		0.0f
	};
	bool isOnRiver = false;
	int isOnRiverTimer = 0;
#pragma endregion
#pragma region バナーの変数 引数
	Vector2 Banner_Stage1 = {
		1280,
		328
	};
	Vector2 Banner_Stage2 = {
		1280,
		328
	};
	Vector2 Banner_Boss = {
		1280,
		328
	};
	float t_Stage1 = 0.0f;
	float t_Stage2 = 0.0f;
	float t_StageBoss = 0.0f;
#pragma endregion
#pragma endregion
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
#pragma region 金钱位数计算  お金の桁計算の更新
		//=============================================
		// 金钱
		//=============================================

		//千位
		eachMoneyNumber[0] = moneyNumber % 10000 / 1000;
		//百位
		eachMoneyNumber[1] = moneyNumber % 1000 / 100;
		//十位
		eachMoneyNumber[2] = moneyNumber % 100 / 10;
		//个位
		eachMoneyNumber[3] = moneyNumber % 10;
#pragma endregion
#pragma region 计时位数计算  タイマーの桁の更新
		//=============================================
		// 计时器
		//=============================================

		//十位
		eachTimerNumber[0] = static_cast<int>(gameTimer) % 100 / 10;
		//个位
		eachTimerNumber[1] = static_cast<int>(gameTimer) % 10;
#pragma endregion
#pragma region シーンの切り替えの更新
		//=============================================
		// シーンの切り替え
		//=============================================
		//START to LOADING
		if (scene == START) {
			Novice::StopAudio(voiceHandle_Stage1);
			Novice::StopAudio(voiceHandle_Stage2);
			Novice::StopAudio(voiceHandle_StageBoss);
			if (Novice::IsPlayingAudio(voiceHandle_StageStart) == 0 || voiceHandle_StageStart == -1) {
				voiceHandle_StageStart = Novice::PlayAudio(SOUND_BGM_STAGESTART, true, 0.7f);
			}
			if (StartSelect == GAMESTART_SELECT) {
				if (keys[DIK_S] && preKeys[DIK_S] == 0 || keys[DIK_DOWN] && preKeys[DIK_DOWN] == 0) {
					StartSelect = CHARACTER_SELECT;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (StartSelect == CHARACTER_SELECT) {
				if (keys[DIK_S] && preKeys[DIK_S] == 0 || keys[DIK_DOWN] && preKeys[DIK_DOWN] == 0) {
					StartSelect = OPTION_SELECT;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			if (StartSelect == CHARACTER_SELECT) {
				if (keys[DIK_W] && preKeys[DIK_W] == 0 || keys[DIK_UP] && preKeys[DIK_UP] == 0) {
					StartSelect = GAMESTART_SELECT;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (StartSelect == OPTION_SELECT) {
				if (keys[DIK_W] && preKeys[DIK_W] == 0 || keys[DIK_UP] && preKeys[DIK_UP] == 0) {
					StartSelect = CHARACTER_SELECT;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}

			if (StartSelect == GAMESTART_SELECT && keys[DIK_RETURN]) {
				voiceHandle_Confirm1 = Novice::PlayAudio(SOUND_CONFIRM1, false, 0.5f);
				scene = LOADING;

			}
			if (StartSelect == CHARACTER_SELECT && keys[DIK_RETURN]) {
				voiceHandle_Confirm1 = Novice::PlayAudio(SOUND_CONFIRM1, false, 0.5f);
				scene = LOADING;

				CharacterIntroduction = WARRIOR_INTRODUCTION;
			}
			if (StartSelect == OPTION_SELECT && keys[DIK_RETURN]) {
				voiceHandle_Confirm1 = Novice::PlayAudio(SOUND_CONFIRM1, false, 0.5f);
				scene = LOADING;

			}

		}

		//LOADING to Others
		else if (scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (StartSelect == GAMESTART_SELECT && SCENE_CHANGE_TIMER <= 0) {
				scene = STAGE1;
				SCENE_CHANGE_TIMER = 60;
			}
			if (StartSelect == CHARACTER_SELECT && SCENE_CHANGE_TIMER <= 0) {
				scene = CHARACTER;
				SCENE_CHANGE_TIMER = 60;
			}
			if (StartSelect == OPTION_SELECT && SCENE_CHANGE_TIMER <= 0) {
				scene = OPTION;
				SCENE_CHANGE_TIMER = 60;
			}
		}

		//Character to start
		//Character to Character
		if (scene == CHARACTER) {
			if (CharacterIntroduction == WARRIOR_INTRODUCTION) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
					CharacterIntroduction = MAGE_INTRODUCTION;
				}
			}
			else if (CharacterIntroduction == MAGE_INTRODUCTION) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
					CharacterIntroduction = PRIEST_INTRODUCTION;
				}
			}
			if (CharacterIntroduction == MAGE_INTRODUCTION) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
					CharacterIntroduction = WARRIOR_INTRODUCTION;
				}
			}
			else if (CharacterIntroduction == PRIEST_INTRODUCTION) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
					CharacterIntroduction = MAGE_INTRODUCTION;
				}
			}
			if (keys[DIK_BACK]) {
				scene = LOADING;
				preScene = CHARACTER;
			}
		}
		else if (preScene == CHARACTER && scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				scene = START;
				SCENE_CHANGE_TIMER = 60;
			}
		}

		//Option to start 
		if (scene == OPTION) {
			if (keys[DIK_BACK]) {
				scene = LOADING;
				preScene = OPTION;
			}
		}
		else if (preScene == OPTION && scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				scene = START;
				SCENE_CHANGE_TIMER = 60;
			}
		}
#pragma endregion
#pragma region ゲーム中の更新
		//=============================================
		// ゲーム中
		//=============================================

		if (!isStagePassed && (scene == STAGE1 || scene == STAGE2 || scene == BOSS)) {
#pragma region 音声
			if (scene == STAGE1) {
				Novice::StopAudio(voiceHandle_StageStart);
				Novice::StopAudio(voiceHandle_Stage2);
				Novice::StopAudio(voiceHandle_StageBoss);
				if (Novice::IsPlayingAudio(voiceHandle_Stage1) == 0 || voiceHandle_Stage1 == -1) {
					voiceHandle_Stage1 = Novice::PlayAudio(SOUND_BGM_STAGE1, true, 0.3f);
				}
			}
			if (scene == STAGE2) {
				Novice::StopAudio(voiceHandle_StageStart);
				Novice::StopAudio(voiceHandle_Stage1);
				Novice::StopAudio(voiceHandle_StageBoss);
				if (Novice::IsPlayingAudio(voiceHandle_Stage2) == 0 || voiceHandle_Stage2 == -1) {
					voiceHandle_Stage2 = Novice::PlayAudio(SOUND_BGM_STAGE2, true, 0.4f);
				}
			}
			if (scene == BOSS) {
				Novice::StopAudio(voiceHandle_StageStart);
				Novice::StopAudio(voiceHandle_Stage1);
				Novice::StopAudio(voiceHandle_Stage2);
				if (Novice::IsPlayingAudio(voiceHandle_StageBoss) == 0 || voiceHandle_StageBoss == -1) {
					voiceHandle_StageBoss = Novice::PlayAudio(SOUND_BGM_STAGEBOSS, true, 0.5f);
				}
			}
#pragma endregion
			if (keys[DIK_P] && preKeys[DIK_P] == 0) {
				isPaused = !isPaused;
			}
			if (isPaused == false) {
#pragma region 时间流逝 タイマー
				//=============================================
				// 时间流逝 60fps
				//=============================================
				gameTimer -= 1.0f / 60;
				if (gameTimer < 0) {
					gameTimer = 0;
				}
#pragma endregion
#pragma region 横幅位置计算 バナーの位置
				if (scene == STAGE1) {
					if (gameTimer <= timerValue && gameTimer > 64) {
						Banner_Stage1.x = (1.0f - (1.0f - powf((1.0f - t_Stage1), 3.0f))) * 1280.0f + (1.0f - powf((1.0f - t_Stage1), 3.0f)) * 0.0f;
						if (t_Stage1 < 1.0f) {
							t_Stage1 += 1.0f / 90.0f;

						}
					}
					else if (gameTimer <= 64 && gameTimer > 62) {
						Banner_Stage1.x = 0;
						t_Stage1 = 0;
					}
					else if (gameTimer <= 62 && gameTimer > 60) {
						Banner_Stage1.x = (1 - powf(t_Stage1, 2)) * 0.0f + powf(t_Stage1, 2) * -1300.0f;
						if (t_Stage1 < 1.0f) {
							t_Stage1 += 1.0f / 90.0f;

						}
					}
				}
				if (scene == STAGE2) {
					if (gameTimer <= timerValue && gameTimer > 64) {
						Banner_Stage2.x = (1.0f - (1.0f - powf((1.0f - t_Stage2), 3.0f))) * 1280.0f + (1.0f - powf((1.0f - t_Stage2), 3.0f)) * 0.0f;
						if (t_Stage2 < 1.0f) {
							t_Stage2 += 1.0f / 90.0f;

						}
					}
					else if (gameTimer <= 64 && gameTimer > 62) {
						Banner_Stage2.x = 0;
						t_Stage2 = 0;
					}
					else if (gameTimer <= 62 && gameTimer > 60) {
						Banner_Stage2.x = (1 - powf(t_Stage2, 2)) * 0.0f + powf(t_Stage2, 2) * -1300.0f;
						if (t_Stage2 < 1.0f) {
							t_Stage2 += 1.0f / 90.0f;

						}
					}
				}
				if (scene == BOSS) {
					if (gameTimer <= timerValue && gameTimer > 64) {
						Banner_Boss.x = (1.0f - (1.0f - powf((1.0f - t_StageBoss), 3.0f))) * 1280.0f + (1.0f - powf((1.0f - t_StageBoss), 3.0f)) * 0.0f;
						if (t_StageBoss < 1.0f) {
							t_StageBoss += 1.0f / 90.0f;

						}
					}
					else if (gameTimer <= 64 && gameTimer > 62) {
						Banner_Boss.x = 0;
						t_StageBoss = 0;
					}
					else if (gameTimer <= 62 && gameTimer > 60) {
						Banner_Boss.x = (1 - powf(t_StageBoss, 2)) * 0.0f + powf(t_StageBoss, 2) * -1300.0f;
						if (t_StageBoss < 1.0f) {
							t_StageBoss += 1.0f / 90.0f;

						}
					}
				}
#pragma endregion
#pragma region 经验值和升级 Exp.とLevel Up
				//经验值和升级
				if (player.level < player.levelMax) {
					if (player.experience < player.levelUPExperience[player.level + 1]) {
						player.experienceMax = player.levelUPExperience[player.level + 1];
					}
					else {
						player.level++;
						player.experience = player.experience - player.levelUPExperience[player.level];
						player.lifePointMax += 5.0f;
						player.lifePoint = player.lifePointMax;
						player.attack += 1.0f;
						player.defense += 1.0f;

					}

				}
#pragma endregion
#pragma region 人物スイッチ
				//=============================================
				// 人物切换
				//=============================================

				if (keys[DIK_1] && preKeys[DIK_1] == 0) {
					Player_character = WARRIOR;
					Player_attribute = PHYSICS;
					player.job = Player_character;
					player.attribute = Player_attribute;
				}
				if (keys[DIK_2] && preKeys[DIK_2] == 0) {
					Player_character = MAGE;
					Player_attribute = MAGIC;
					player.job = Player_character;
					player.attribute = Player_attribute;
				}
				if (keys[DIK_3] && preKeys[DIK_3] == 0) {
					Player_character = PRIEST;
					Player_attribute = HOLY;
					player.job = Player_character;
					player.attribute = Player_attribute;
				}
#pragma endregion
#pragma region 八方向移動
				//=============================================
				// 八方向移動
				//=============================================

				//正規化
				player.velocityLength = sqrtf(player.moveVelocityMax.x * player.moveVelocityMax.x +
					player.moveVelocityMax.y * player.moveVelocityMax.y);
				if (player.velocityLength != 0) {
					player.normalizeVelocity.x = player.moveVelocityMax.x / player.velocityLength;
					player.normalizeVelocity.y = player.moveVelocityMax.y / player.velocityLength;
				}

				// 移動
				if (keys[DIK_W] && keys[DIK_A] || keys[DIK_UP] && keys[DIK_LEFT]) {
					player.isTurnRight = false;
					player.Position.x -= player.normalizeVelocity.x * player.moveVelocityMax.x;
					player.Position.y -= player.normalizeVelocity.y * player.moveVelocityMax.y;
				}
				else if (keys[DIK_W] && keys[DIK_D] || keys[DIK_UP] && keys[DIK_RIGHT]) {
					player.isTurnRight = true;
					player.Position.x += player.normalizeVelocity.x * player.moveVelocityMax.x;
					player.Position.y -= player.normalizeVelocity.y * player.moveVelocityMax.y;
				}
				else if (keys[DIK_S] && keys[DIK_A] || keys[DIK_DOWN] && keys[DIK_LEFT]) {
					player.isTurnRight = false;
					player.Position.x -= player.normalizeVelocity.x * player.moveVelocityMax.x;
					player.Position.y += player.normalizeVelocity.y * player.moveVelocityMax.y;
				}
				else if (keys[DIK_S] && keys[DIK_D] || keys[DIK_DOWN] && keys[DIK_RIGHT]) {
					player.isTurnRight = true;
					player.Position.x += player.normalizeVelocity.x * player.moveVelocityMax.x;
					player.Position.y += player.normalizeVelocity.y * player.moveVelocityMax.y;
				}

				else if (keys[DIK_W] || keys[DIK_UP]) {
					player.Position.y -= player.moveVelocityMax.y;
				}

				else if (keys[DIK_S] || keys[DIK_DOWN]) {
					player.Position.y += player.moveVelocityMax.y;
				}

				else if (keys[DIK_A] || keys[DIK_LEFT]) {
					player.isTurnRight = false;
					player.Position.x -= player.moveVelocityMax.x;
				}

				else if (keys[DIK_D] || keys[DIK_RIGHT]) {
					player.isTurnRight = true;
					player.Position.x += player.moveVelocityMax.x;
				}
				// 界限
				if (player.Position.x < 0) {
					player.Position.x = 0;
				}
				if (player.Position.x > 1280 - 64) {
					player.Position.x = 1280 - 64;
				}
				if (player.Position.y < 0) {
					player.Position.y = 0;
				}
				if (player.Position.y > 720 - 64) {
					player.Position.y = 720 - 64;
				}
#pragma endregion
#pragma region 敵の行為
#pragma region 敌人复活(重要调整) 敵のリスポーン
				//=============================================
				// 复活 此处可以调整各个怪物 生成数量 和 生成时间段
				//=============================================
				if (scene == STAGE1) {
#pragma region goblin复活
					//goblin
					if (gameTimer < timerValue) {//生成时间段
						for (int i = 0; i < 10; i++) {//生成数量
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 10; i < 20; i++) {
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 20; i < 25; i++) {
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}

#pragma endregion
#pragma region Ghost复活
					//Ghost
					if (gameTimer < timerValue) {
						for (int i = 0; i < 3; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 3; i < 7; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 7; i < 10; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Demon复活
					//Demon
					if (gameTimer < timerValue) {
						for (int i = 0; i < 3; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 3; i < 7; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 7; i < 15; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Dragon复活
					//Dragon
					if (gameTimer < timerValue) {
						for (int i = 0; i < 1; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 1; i < 2; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 2; i < 3; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
				}
				if (scene == STAGE2) {
#pragma region goblin复活
					//goblin
					if (gameTimer < timerValue) {//生成时间段
						for (int i = 0; i < 15; i++) {//生成数量
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 15; i < 25; i++) {
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 25; i < 30; i++) {
							if (Goblin[i].isEnemyRealAlive) {
								if (Goblin[i].isEnemyAlive == false) {
									if (Goblin[i].respawnTimer > 0) {
										Goblin[i].respawnTimer--;
									}
									else {
										Goblin[i].isEnemyAlive = true;
										Goblin[i].respawnTimer = respawnTimerValue;
										Goblin[i].lifePoint = Goblin[i].lifePointMax;
										Goblin[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Goblin[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Goblin[i].isMove == false && Goblin[i].enemyAppearTimer > 0) {
										Goblin[i].enemyAppearTimer--;
									}
									else {
										Goblin[i].isMove = true;
										Goblin[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}

#pragma endregion
#pragma region Ghost复活
					//Ghost
					if (gameTimer < timerValue) {
						for (int i = 0; i < 5; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 5; i < 8; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 8; i < 12; i++) {
							if (Ghost[i].isEnemyRealAlive) {
								if (Ghost[i].isEnemyAlive == false) {
									if (Ghost[i].respawnTimer > 0) {
										Ghost[i].respawnTimer--;
									}
									else {
										Ghost[i].isEnemyAlive = true;
										Ghost[i].respawnTimer = respawnTimerValue;
										Ghost[i].lifePoint = Ghost[i].lifePointMax;
										Ghost[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Ghost[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Ghost[i].isMove == false && Ghost[i].enemyAppearTimer > 0) {
										Ghost[i].enemyAppearTimer--;
									}
									else {
										Ghost[i].isMove = true;
										Ghost[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Demon复活
					//Demon
					if (gameTimer < timerValue) {
						for (int i = 0; i < 3; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 3; i < 7; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 7; i < 15; i++) {
							if (Demon[i].isEnemyRealAlive) {
								if (Demon[i].isEnemyAlive == false) {
									if (Demon[i].respawnTimer > 0) {
										Demon[i].respawnTimer--;
									}
									else {
										Demon[i].isEnemyAlive = true;
										Demon[i].respawnTimer = respawnTimerValue;
										Demon[i].lifePoint = Demon[i].lifePointMax;
										Demon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Demon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Demon[i].isMove == false && Demon[i].enemyAppearTimer > 0) {
										Demon[i].enemyAppearTimer--;
									}
									else {
										Demon[i].isMove = true;
										Demon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
#pragma region Dragon复活
					//Dragon
					if (gameTimer < timerValue) {
						for (int i = 0; i < 1; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 46) {
						for (int i = 1; i < 3; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
					if (gameTimer < 26) {
						for (int i = 3; i < 5; i++) {
							if (Dragon[i].isEnemyRealAlive) {
								if (Dragon[i].isEnemyAlive == false) {
									if (Dragon[i].respawnTimer > 0) {
										Dragon[i].respawnTimer--;
									}
									else {
										Dragon[i].isEnemyAlive = true;
										Dragon[i].respawnTimer = respawnTimerValue;
										Dragon[i].lifePoint = Dragon[i].lifePointMax;
										Dragon[i].Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
										Dragon[i].Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
									}
								}
								else {
									if (Dragon[i].isMove == false && Dragon[i].enemyAppearTimer > 0) {
										Dragon[i].enemyAppearTimer--;
									}
									else {
										Dragon[i].isMove = true;
										Dragon[i].enemyAppearTimer = enemyAppearTimerValue;
									}
								}
							}
						}
					}
#pragma endregion
				}
				if (scene == BOSS) {
#pragma region boss复活
					if (gameTimer < timerValue) {//生成时间段
						if (Boss.isEnemyRealAlive) {
							if (Boss.isEnemyAlive == false) {
								if (Boss.respawnTimer > 0) {
									Boss.respawnTimer--;
								}
								else {
									Boss.isEnemyAlive = true;
									Boss.respawnTimer = respawnTimerValue;
									Boss.lifePoint = Boss.lifePointMax;
									Boss.Position.x = (rand() % 2 == 0) ? (float)(rand() % 500) : (float)(rand() % 437 + 780);
									Boss.Position.y = (rand() % 2 == 0) ? (float)(rand() % 250) : (float)(rand() % 187 + 470);
								}
							}
							else {
								if (Boss.isMove == false && Boss.enemyAppearTimer > 0) {
									Boss.enemyAppearTimer--;
								}
								else {
									Boss.isMove = true;
									Boss.enemyAppearTimer = enemyAppearTimerValue;
								}
							}
						}
					}
#pragma endregion
				}
#pragma endregion
#pragma region 敌人移动 正规化 敵の移動と正規化
				//=============================================
				// 移动 正规化
				//=============================================
				if (scene == STAGE1 || scene == STAGE2) {
#pragma region 杂兵正规化和移动
					//goblin
					for (int i = 0; i < 50; i++) {
						if (Goblin[i].isEnemyRealAlive) {
							if (Goblin[i].isEnemyAlive && Goblin[i].isMove) {
								Goblin[i].vector.x = player.Position.x - Goblin[i].Position.x;
								Goblin[i].vector.y = player.Position.y - Goblin[i].Position.y;
								Goblin[i].length = sqrtf(Goblin[i].vector.x * Goblin[i].vector.x + Goblin[i].vector.y * Goblin[i].vector.y);
								if (Goblin[i].length != 0) {
									Goblin[i].vectorNew.x = Goblin[i].vector.x / Goblin[i].length;
									Goblin[i].vectorNew.y = Goblin[i].vector.y / Goblin[i].length;
								}
								else {
									Goblin[i].vectorNew.x = 0;
									Goblin[i].vectorNew.y = 0;
								}
							}
						}
					}
					//ghost
					for (int i = 0; i < 50; i++) {
						if (Ghost[i].isEnemyRealAlive) {
							if (Ghost[i].isEnemyAlive && Ghost[i].isMove) {
								Ghost[i].vector.x = player.Position.x - Ghost[i].Position.x;
								Ghost[i].vector.y = player.Position.y - Ghost[i].Position.y;
								Ghost[i].length = sqrtf(Ghost[i].vector.x * Ghost[i].vector.x + Ghost[i].vector.y * Ghost[i].vector.y);
								if (Ghost[i].length != 0) {
									Ghost[i].vectorNew.x = Ghost[i].vector.x / Ghost[i].length;
									Ghost[i].vectorNew.y = Ghost[i].vector.y / Ghost[i].length;
								}
								else {
									Ghost[i].vectorNew.x = 0;
									Ghost[i].vectorNew.y = 0;
								}
							}
						}
					}
					//demon
					for (int i = 0; i < 50; i++) {
						if (Demon[i].isEnemyRealAlive) {
							if (Demon[i].isEnemyAlive && Demon[i].isMove) {
								Demon[i].vector.x = player.Position.x - Demon[i].Position.x;
								Demon[i].vector.y = player.Position.y - Demon[i].Position.y;
								Demon[i].length = sqrtf(Demon[i].vector.x * Demon[i].vector.x + Demon[i].vector.y * Demon[i].vector.y);
								if (Demon[i].length != 0) {
									Demon[i].vectorNew.x = Demon[i].vector.x / Demon[i].length;
									Demon[i].vectorNew.y = Demon[i].vector.y / Demon[i].length;
								}
								else {
									Demon[i].vectorNew.x = 0;
									Demon[i].vectorNew.y = 0;
								}
							}
						}
					}

					//dragon
					for (int i = 0; i < 50; i++) {
						if (Dragon[i].isEnemyRealAlive) {
							if (Dragon[i].isEnemyAlive && Dragon[i].isMove) {
								Dragon[i].vector.x = player.Position.x - Dragon[i].Position.x;
								Dragon[i].vector.y = player.Position.y - Dragon[i].Position.y;
								Dragon[i].length = sqrtf(Dragon[i].vector.x * Dragon[i].vector.x + Dragon[i].vector.y * Dragon[i].vector.y);
								if (Dragon[i].length != 0) {
									Dragon[i].vectorNew.x = Dragon[i].vector.x / Dragon[i].length;
									Dragon[i].vectorNew.y = Dragon[i].vector.y / Dragon[i].length;
								}
								else {
									Dragon[i].vectorNew.x = 0;
									Dragon[i].vectorNew.y = 0;
								}
							}
						}
					}
					//=============================================
					// 生成敌人后 向自己移动
					//=============================================
					//goblin
					for (int i = 0; i < 50; i++) {
						if (Goblin[i].isEnemyRealAlive) {
							if (Goblin[i].isEnemyAlive && Goblin[i].isMove) {
								Goblin[i].Position.x += Goblin[i].vectorNew.x * Goblin[i].moveVelocity.x;
								Goblin[i].Position.y += Goblin[i].vectorNew.y * Goblin[i].moveVelocity.y;
							}
						}
					}
					//ghost
					for (int i = 0; i < 50; i++) {
						if (Ghost[i].isEnemyRealAlive) {
							if (Ghost[i].isEnemyAlive && Ghost[i].isMove) {
								Ghost[i].Position.x += Ghost[i].vectorNew.x * Ghost[i].moveVelocity.x;
								Ghost[i].Position.y += Ghost[i].vectorNew.y * Ghost[i].moveVelocity.y;
							}
						}
					}
					//demon
					for (int i = 0; i < 50; i++) {
						if (Demon[i].isEnemyRealAlive) {
							if (Demon[i].isEnemyAlive && Demon[i].isMove) {
								Demon[i].Position.x += Demon[i].vectorNew.x * Demon[i].moveVelocity.x;
								Demon[i].Position.y += Demon[i].vectorNew.y * Demon[i].moveVelocity.y;
							}
						}
					}
					//dragon
					for (int i = 0; i < 50; i++) {
						if (Dragon[i].isEnemyRealAlive) {
							if (Dragon[i].isEnemyAlive && Dragon[i].isMove) {
								Dragon[i].Position.x += Dragon[i].vectorNew.x * Dragon[i].moveVelocity.x;
								Dragon[i].Position.y += Dragon[i].vectorNew.y * Dragon[i].moveVelocity.y;
							}
						}
					}
#pragma endregion
				}
				if (scene == BOSS) {
#pragma region boss移动 正规化
					//正规化
					if (Boss.isEnemyRealAlive) {
						if (Boss.isEnemyAlive && Boss.isMove) {
							Boss.vector.x = player.Position.x - Boss.Position.x;
							Boss.vector.y = player.Position.y - Boss.Position.y;
							Boss.length = sqrtf(Boss.vector.x * Boss.vector.x + Boss.vector.y * Boss.vector.y);
							if (Boss.length != 0) {
								Boss.vectorNew.x = Boss.vector.x / Boss.length;
								Boss.vectorNew.y = Boss.vector.y / Boss.length;
							}
							else {
								Boss.vectorNew.x = 0;
								Boss.vectorNew.y = 0;
							}
						}
					}
					//移动
					if (Boss.isEnemyRealAlive) {
						if (Boss.isEnemyAlive && Boss.isMove) {
							Boss.Position.x += Boss.vectorNew.x * Boss.moveVelocity.x;
							Boss.Position.y += Boss.vectorNew.y * Boss.moveVelocity.y;
						}
					}
#pragma endregion
				}

#pragma endregion
#pragma endregion
#pragma region 射击部分 シューティング
				//自己射击
#pragma region 自机和敌人中心距离判断 距離計算
				//boss
				if (Boss.isEnemyAlive && Boss.isEnemyRealAlive && Boss.isMove) {
					dinstanceMin5 = sqrtf(
						((Boss.Position.x + Boss.width / 2) - (player.Position.x + player.width / 2)) *
						((Boss.Position.x + Boss.width / 2) - (player.Position.x + player.width / 2)) +
						((Boss.Position.y + Boss.height / 2) - (player.Position.y + player.height / 2)) *
						((Boss.Position.y + Boss.height / 2) - (player.Position.y + player.height / 2))
					);
				}
				else {
					dinstanceMin5 = 1300;
				}

				//goblin
				for (int i = 0; i < 50; i++) {
					if (Goblin[i].isEnemyAlive && Goblin[i].isEnemyRealAlive && Goblin[i].isMove) {
						distance1[i] = sqrtf(
							((Goblin[i].Position.x + Goblin[i].width / 2) - (player.Position.x + player.width / 2)) *
							((Goblin[i].Position.x + Goblin[i].width / 2) - (player.Position.x + player.width / 2)) +
							((Goblin[i].Position.y + Goblin[i].height / 2) - (player.Position.y + player.height / 2)) *
							((Goblin[i].Position.y + Goblin[i].height / 2) - (player.Position.y + player.height / 2))
						);
					}
					else {
						distance1[i] = 1300;
					}
					//ghost
					if (Ghost[i].isEnemyAlive && Ghost[i].isEnemyRealAlive && Ghost[i].isMove) {
						distance2[i] = sqrtf(
							((Ghost[i].Position.x + Ghost[i].width / 2) - (player.Position.x + player.width / 2)) *
							((Ghost[i].Position.x + Ghost[i].width / 2) - (player.Position.x + player.width / 2)) +
							((Ghost[i].Position.y + Ghost[i].height / 2) - (player.Position.y + player.height / 2)) *
							((Ghost[i].Position.y + Ghost[i].height / 2) - (player.Position.y + player.height / 2))
						);
					}
					else {
						distance2[i] = 1300;
					}
					//demon
					if (Demon[i].isEnemyAlive && Demon[i].isEnemyRealAlive && Demon[i].isMove) {
						distance3[i] = sqrtf(
							((Demon[i].Position.x + Demon[i].width / 2) - (player.Position.x + player.width / 2)) *
							((Demon[i].Position.x + Demon[i].width / 2) - (player.Position.x + player.width / 2)) +
							((Demon[i].Position.y + Demon[i].height / 2) - (player.Position.y + player.height / 2)) *
							((Demon[i].Position.y + Demon[i].height / 2) - (player.Position.y + player.height / 2))
						);
					}
					else {
						distance3[i] = 1300;
					}
					//dragon
					if (Dragon[i].isEnemyAlive && Dragon[i].isEnemyRealAlive && Dragon[i].isMove) {
						distance4[i] = sqrtf(
							((Dragon[i].Position.x + Dragon[i].width / 2) - (player.Position.x + player.width / 2)) *
							((Dragon[i].Position.x + Dragon[i].width / 2) - (player.Position.x + player.width / 2)) +
							((Dragon[i].Position.y + Dragon[i].height / 2) - (player.Position.y + player.height / 2)) *
							((Dragon[i].Position.y + Dragon[i].height / 2) - (player.Position.y + player.height / 2))
						);
					}
					else {
						distance4[i] = 1300;
					}
				}
				size1 = sizeof(distance1) / sizeof(distance1[0]);
				size2 = sizeof(distance2) / sizeof(distance2[0]);
				size3 = sizeof(distance3) / sizeof(distance3[0]);
				size4 = sizeof(distance4) / sizeof(distance4[0]);
				dinstanceMin1 = findMin(distance1, size1, &dMin1);
				dinstanceMin2 = findMin(distance2, size2, &dMin2);
				dinstanceMin3 = findMin(distance3, size3, &dMin3);
				dinstanceMin4 = findMin(distance4, size4, &dMin4);
				dinstanceMinOfMin = minOfFive(dinstanceMin1, dinstanceMin2, dinstanceMin3, dinstanceMin4, dinstanceMin5);
#pragma endregion
#pragma region 複数弾
				//复数弹
#pragma region 战士
			//战士
			//装弹
				if (player.job == WARRIOR) {
					for (int i = 0; i < 50; i++) {
						mageBullet[i].isExist = false;
						mageBullet[i].isShoot = false;
						priestBullet[i].isExist = false;
						priestBullet[i].isShoot = false;
					}
					if (player.shootCoolTime > 0) {
						player.shootCoolTime--;
					}
					else {
						player.shootCoolTime = 10;
					}
					if (player.shootCoolTime == 0) {
						if (dinstanceMinOfMin <= 300) {//射程
							for (int i = 0; i < 50; i++) {
								if (Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive && Goblin[i].isMove ||
									Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive && Ghost[i].isMove ||
									Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive && Demon[i].isMove ||
									Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive && Dragon[i].isMove ||
									Boss.isEnemyRealAlive && Boss.isEnemyAlive && Boss.isMove) {
									if (warriorBullet[i].isShoot == false) {
										warriorBullet[i].Position.x = (player.Position.x + player.width / 2);
										warriorBullet[i].Position.y = (player.Position.y + player.height / 2);
										warriorBullet[i].isExist = true;
										warriorBullet[i].isShoot = true;
										warriorBullet[i].existTime = 10;
										break;
									}
								}
							}
						}
					}

					//发射
					for (int i = 0; i < 50; i++) {
						warriorBullet[i].existTime--;
						if (warriorBullet[i].existTime <= 0) {
							warriorBullet[i].isExist = false;
							warriorBullet[i].isShoot = false;
						}
						if (warriorBullet[i].isShoot && warriorBullet[i].isExist) {
							if (dinstanceMinOfMin == dinstanceMin1) {
								warriorBullet[i].velocityVector.x = (Goblin[dMin1].Position.x + Goblin[dMin1].width / 2) - (warriorBullet[i].Position.x + warriorBullet[i].width / 2);
								warriorBullet[i].velocityVector.y = (Goblin[dMin1].Position.y + Goblin[dMin1].height / 2) - (warriorBullet[i].Position.y + warriorBullet[i].height / 2);
								warriorBullet[i].length = sqrtf(
									warriorBullet[i].velocityVector.x * warriorBullet[i].velocityVector.x +
									warriorBullet[i].velocityVector.y * warriorBullet[i].velocityVector.y
								);
								if (warriorBullet[i].length > 0) {
									warriorBullet[i].velocityVectorNew.x = warriorBullet[i].velocityVector.x / warriorBullet[i].length;
									warriorBullet[i].velocityVectorNew.y = warriorBullet[i].velocityVector.y / warriorBullet[i].length;
									warriorBullet[i].Position.x += warriorBullet[i].velocityVectorNew.x * warriorBullet[i].velocity;
									warriorBullet[i].Position.y += warriorBullet[i].velocityVectorNew.y * warriorBullet[i].velocity;
								}
								else {
									warriorBullet[i].isExist = false;
									warriorBullet[i].isShoot = false;
								}
							}

							if (dinstanceMinOfMin == dinstanceMin2) {
								warriorBullet[i].velocityVector.x = (Ghost[dMin2].Position.x + Ghost[dMin2].width / 2) - (player.Position.x + player.width / 2);
								warriorBullet[i].velocityVector.y = (Ghost[dMin2].Position.y + Ghost[dMin2].height / 2) - (player.Position.y + player.height / 2);
								warriorBullet[i].length = sqrtf(
									warriorBullet[i].velocityVector.x * warriorBullet[i].velocityVector.x +
									warriorBullet[i].velocityVector.y * warriorBullet[i].velocityVector.y
								);

								if (warriorBullet[i].length > 0) {
									warriorBullet[i].velocityVectorNew.x = warriorBullet[i].velocityVector.x / warriorBullet[i].length;
									warriorBullet[i].velocityVectorNew.y = warriorBullet[i].velocityVector.y / warriorBullet[i].length;
									warriorBullet[i].Position.x += warriorBullet[i].velocityVectorNew.x * warriorBullet[i].velocity;
									warriorBullet[i].Position.y += warriorBullet[i].velocityVectorNew.y * warriorBullet[i].velocity;
								}
								else {
									warriorBullet[i].isExist = false;
									warriorBullet[i].isShoot = false;
								}
							}
							if (dinstanceMinOfMin == dinstanceMin3) {
								warriorBullet[i].velocityVector.x = (Demon[dMin3].Position.x + Demon[dMin3].width / 2) - (player.Position.x + player.width / 2);
								warriorBullet[i].velocityVector.y = (Demon[dMin3].Position.y + Demon[dMin3].height / 2) - (player.Position.y + player.height / 2);
								warriorBullet[i].length = sqrtf(
									warriorBullet[i].velocityVector.x * warriorBullet[i].velocityVector.x +
									warriorBullet[i].velocityVector.y * warriorBullet[i].velocityVector.y
								);

								if (warriorBullet[i].length > 0) {
									warriorBullet[i].velocityVectorNew.x = warriorBullet[i].velocityVector.x / warriorBullet[i].length;
									warriorBullet[i].velocityVectorNew.y = warriorBullet[i].velocityVector.y / warriorBullet[i].length;
									warriorBullet[i].Position.x += warriorBullet[i].velocityVectorNew.x * warriorBullet[i].velocity;
									warriorBullet[i].Position.y += warriorBullet[i].velocityVectorNew.y * warriorBullet[i].velocity;
								}
								else {
									warriorBullet[i].isExist = false;
									warriorBullet[i].isShoot = false;
								}
							}
							if (dinstanceMinOfMin == dinstanceMin4) {
								warriorBullet[i].velocityVector.x = (Dragon[dMin4].Position.x + Dragon[dMin4].width / 2) - (player.Position.x + player.width / 2);
								warriorBullet[i].velocityVector.y = (Dragon[dMin4].Position.y + Dragon[dMin4].height / 2) - (player.Position.y + player.height / 2);
								warriorBullet[i].length = sqrtf(
									warriorBullet[i].velocityVector.x * warriorBullet[i].velocityVector.x +
									warriorBullet[i].velocityVector.y * warriorBullet[i].velocityVector.y
								);

								if (warriorBullet[i].length > 0) {
									warriorBullet[i].velocityVectorNew.x = warriorBullet[i].velocityVector.x / warriorBullet[i].length;
									warriorBullet[i].velocityVectorNew.y = warriorBullet[i].velocityVector.y / warriorBullet[i].length;
									warriorBullet[i].Position.x += warriorBullet[i].velocityVectorNew.x * warriorBullet[i].velocity;
									warriorBullet[i].Position.y += warriorBullet[i].velocityVectorNew.y * warriorBullet[i].velocity;
								}
								else {
									warriorBullet[i].isExist = false;
									warriorBullet[i].isShoot = false;
								}
							}
							if (dinstanceMinOfMin == dinstanceMin5) {
								warriorBullet[i].velocityVector.x = (Boss.Position.x + Boss.width / 2) - (player.Position.x + player.width / 2);
								warriorBullet[i].velocityVector.y = (Boss.Position.y + Boss.height / 2) - (player.Position.y + player.height / 2);
								warriorBullet[i].length = sqrtf(
									warriorBullet[i].velocityVector.x * warriorBullet[i].velocityVector.x +
									warriorBullet[i].velocityVector.y * warriorBullet[i].velocityVector.y
								);

								if (warriorBullet[i].length > 0) {
									warriorBullet[i].velocityVectorNew.x = warriorBullet[i].velocityVector.x / warriorBullet[i].length;
									warriorBullet[i].velocityVectorNew.y = warriorBullet[i].velocityVector.y / warriorBullet[i].length;
									warriorBullet[i].Position.x += warriorBullet[i].velocityVectorNew.x * warriorBullet[i].velocity;
									warriorBullet[i].Position.y += warriorBullet[i].velocityVectorNew.y * warriorBullet[i].velocity;
								}
								else {
									warriorBullet[i].isExist = false;
									warriorBullet[i].isShoot = false;
								}
							}
						}
						if (warriorBullet[i].Position.x < 0 || warriorBullet[i].Position.x + warriorBullet[i].width > 1280 ||
							warriorBullet[i].Position.y < 0 || warriorBullet[i].Position.y + warriorBullet[i].height > 720 ||
							warriorBullet[i].velocityVectorNew.x == 0 && warriorBullet[i].velocityVectorNew.y == 0) {
							warriorBullet[i].isExist = false;
							warriorBullet[i].isShoot = false;
						}
					}
				}

#pragma endregion			
#pragma region 法师
				//法师
				//装弹
				if (player.job == MAGE) {
					for (int i = 0; i < 50; i++) {
						warriorBullet[i].isExist = false;
						warriorBullet[i].isShoot = false;
						priestBullet[i].isExist = false;
						priestBullet[i].isShoot = false;
					}
					if (player.shootCoolTime > 0) {
						player.shootCoolTime--;
					}
					else {
						player.shootCoolTime = 10;
					}
					if (player.shootCoolTime == 0) {
						if (dinstanceMinOfMin <= 450) {//射程
							for (int i = 0; i < 50; i++) {
								if (Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive && Goblin[i].isMove ||
									Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive && Ghost[i].isMove ||
									Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive && Demon[i].isMove ||
									Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive && Dragon[i].isMove ||
									Boss.isEnemyRealAlive && Boss.isEnemyAlive && Boss.isMove) {
									if (mageBullet[i].isShoot == false) {
										mageBullet[i].Position.x = (player.Position.x + player.width / 2) - mageBullet[i].width / 2;
										mageBullet[i].Position.y = (player.Position.y + player.height / 2) - mageBullet[i].height / 2;
										mageBullet[i].isExist = true;
										mageBullet[i].isShoot = true;
										mageBullet[i].existTime = 20;
										break;
									}
								}
							}
						}
					}

					//发射
					for (int i = 0; i < 50; i++) {
						mageBullet[i].existTime--;
						if (mageBullet[i].existTime <= 0) {
							mageBullet[i].isExist = false;
							mageBullet[i].isShoot = false;
						}
						if (mageBullet[i].isShoot && mageBullet[i].isExist) {
							if (/*Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive && Goblin[i].isMove && */dinstanceMinOfMin == dinstanceMin1) {
								mageBullet[i].velocityVector.x = (Goblin[dMin1].Position.x + Goblin[dMin1].width / 2) - (mageBullet[i].Position.x + mageBullet[i].width / 2);
								mageBullet[i].velocityVector.y = (Goblin[dMin1].Position.y + Goblin[dMin1].height / 2) - (mageBullet[i].Position.y + mageBullet[i].height / 2);
								mageBullet[i].length = sqrtf(
									mageBullet[i].velocityVector.x * mageBullet[i].velocityVector.x +
									mageBullet[i].velocityVector.y * mageBullet[i].velocityVector.y
								);
								if (mageBullet[i].length > 0) {
									mageBullet[i].velocityVectorNew.x = mageBullet[i].velocityVector.x / mageBullet[i].length;
									mageBullet[i].velocityVectorNew.y = mageBullet[i].velocityVector.y / mageBullet[i].length;
									mageBullet[i].Position.x += mageBullet[i].velocityVectorNew.x * mageBullet[i].velocity;
									mageBullet[i].Position.y += mageBullet[i].velocityVectorNew.y * mageBullet[i].velocity;
								}
								else {
									mageBullet[i].isExist = false;
									mageBullet[i].isShoot = false;
								}
							}

							if (/*Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive && Ghost[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin2) {
								mageBullet[i].velocityVector.x = (Ghost[dMin2].Position.x + Ghost[dMin2].width / 2) - (player.Position.x + player.width / 2);
								mageBullet[i].velocityVector.y = (Ghost[dMin2].Position.y + Ghost[dMin2].height / 2) - (player.Position.y + player.height / 2);
								mageBullet[i].length = sqrtf(
									mageBullet[i].velocityVector.x * mageBullet[i].velocityVector.x +
									mageBullet[i].velocityVector.y * mageBullet[i].velocityVector.y
								);

								if (mageBullet[i].length > 0) {
									mageBullet[i].velocityVectorNew.x = mageBullet[i].velocityVector.x / mageBullet[i].length;
									mageBullet[i].velocityVectorNew.y = mageBullet[i].velocityVector.y / mageBullet[i].length;
									mageBullet[i].Position.x += mageBullet[i].velocityVectorNew.x * mageBullet[i].velocity;
									mageBullet[i].Position.y += mageBullet[i].velocityVectorNew.y * mageBullet[i].velocity;
								}
								else {
									mageBullet[i].isExist = false;
									mageBullet[i].isShoot = false;
								}
							}
							if (/*Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive && Demon[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin3) {
								mageBullet[i].velocityVector.x = (Demon[dMin3].Position.x + Demon[dMin3].width / 2) - (player.Position.x + player.width / 2);
								mageBullet[i].velocityVector.y = (Demon[dMin3].Position.y + Demon[dMin3].height / 2) - (player.Position.y + player.height / 2);
								mageBullet[i].length = sqrtf(
									mageBullet[i].velocityVector.x * mageBullet[i].velocityVector.x +
									mageBullet[i].velocityVector.y * mageBullet[i].velocityVector.y
								);

								if (mageBullet[i].length > 0) {
									mageBullet[i].velocityVectorNew.x = mageBullet[i].velocityVector.x / mageBullet[i].length;
									mageBullet[i].velocityVectorNew.y = mageBullet[i].velocityVector.y / mageBullet[i].length;
									mageBullet[i].Position.x += mageBullet[i].velocityVectorNew.x * mageBullet[i].velocity;
									mageBullet[i].Position.y += mageBullet[i].velocityVectorNew.y * mageBullet[i].velocity;
								}
								else {
									mageBullet[i].isExist = false;
									mageBullet[i].isShoot = false;
								}
							}
							if (/*Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive && Dragon[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin4) {
								mageBullet[i].velocityVector.x = (Dragon[dMin4].Position.x + Dragon[dMin4].width / 2) - (player.Position.x + player.width / 2);
								mageBullet[i].velocityVector.y = (Dragon[dMin4].Position.y + Dragon[dMin4].height / 2) - (player.Position.y + player.height / 2);
								mageBullet[i].length = sqrtf(
									mageBullet[i].velocityVector.x * mageBullet[i].velocityVector.x +
									mageBullet[i].velocityVector.y * mageBullet[i].velocityVector.y
								);

								if (mageBullet[i].length > 0) {
									mageBullet[i].velocityVectorNew.x = mageBullet[i].velocityVector.x / mageBullet[i].length;
									mageBullet[i].velocityVectorNew.y = mageBullet[i].velocityVector.y / mageBullet[i].length;
									mageBullet[i].Position.x += mageBullet[i].velocityVectorNew.x * mageBullet[i].velocity;
									mageBullet[i].Position.y += mageBullet[i].velocityVectorNew.y * mageBullet[i].velocity;
								}
								else {
									mageBullet[i].isExist = false;
									mageBullet[i].isShoot = false;
								}
							}
							if (dinstanceMinOfMin == dinstanceMin5) {
								mageBullet[i].velocityVector.x = (Boss.Position.x + Boss.width / 2) - (player.Position.x + player.width / 2);
								mageBullet[i].velocityVector.y = (Boss.Position.y + Boss.height / 2) - (player.Position.y + player.height / 2);
								mageBullet[i].length = sqrtf(
									mageBullet[i].velocityVector.x * mageBullet[i].velocityVector.x +
									mageBullet[i].velocityVector.y * mageBullet[i].velocityVector.y
								);

								if (mageBullet[i].length > 0) {
									mageBullet[i].velocityVectorNew.x = mageBullet[i].velocityVector.x / mageBullet[i].length;
									mageBullet[i].velocityVectorNew.y = mageBullet[i].velocityVector.y / mageBullet[i].length;
									mageBullet[i].Position.x += mageBullet[i].velocityVectorNew.x * mageBullet[i].velocity;
									mageBullet[i].Position.y += mageBullet[i].velocityVectorNew.y * mageBullet[i].velocity;
								}
								else {
									mageBullet[i].isExist = false;
									mageBullet[i].isShoot = false;
								}
							}

						}
						if (mageBullet[i].Position.x < 0 || mageBullet[i].Position.x + mageBullet[i].width > 1280 ||
							mageBullet[i].Position.y < 0 || mageBullet[i].Position.y + mageBullet[i].height > 720 ||
							mageBullet[i].velocityVectorNew.x == 0 && mageBullet[i].velocityVectorNew.y == 0) {
							mageBullet[i].isExist = false;
							mageBullet[i].isShoot = false;
						}
					}
				}

#pragma endregion
#pragma region 牧师
				//牧师
				//装弹
				if (player.job == PRIEST) {
					for (int i = 0; i < 50; i++) {
						warriorBullet[i].isExist = false;
						warriorBullet[i].isShoot = false;
						mageBullet[i].isExist = false;
						mageBullet[i].isShoot = false;
					}
					if (player.shootCoolTime > 0) {
						player.shootCoolTime--;
					}
					else {
						player.shootCoolTime = 10;
					}
					if (player.shootCoolTime == 0) {
						if (dinstanceMinOfMin <= 300) {//射程
							for (int i = 0; i < 50; i++) {
								if (Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive && Goblin[i].isMove ||
									Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive && Ghost[i].isMove ||
									Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive && Demon[i].isMove ||
									Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive && Dragon[i].isMove ||
									Boss.isEnemyRealAlive && Boss.isEnemyAlive && Boss.isMove) {
									if (priestBullet[i].isShoot == false) {
										priestBullet[i].Position.x = (player.Position.x + player.width / 2) - priestBullet[i].width / 2;
										priestBullet[i].Position.y = (player.Position.y + player.height / 2) - priestBullet[i].height / 2;
										priestBullet[i].isExist = true;
										priestBullet[i].isShoot = true;
										priestBullet[i].existTime = 20;
										break;
									}
								}
							}
						}
					}

					//发射
					for (int i = 0; i < 50; i++) {
						priestBullet[i].existTime--;
						if (priestBullet[i].existTime <= 0) {
							priestBullet[i].isExist = false;
							priestBullet[i].isShoot = false;
						}
						if (priestBullet[i].isShoot && priestBullet[i].isExist) {
							if (/*Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive && Goblin[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin1) {
								priestBullet[i].velocityVector.x = (Goblin[dMin1].Position.x + Goblin[dMin1].width / 2) - (priestBullet[i].Position.x + priestBullet[i].width / 2);
								priestBullet[i].velocityVector.y = (Goblin[dMin1].Position.y + Goblin[dMin1].height / 2) - (priestBullet[i].Position.y + priestBullet[i].height / 2);
								priestBullet[i].length = sqrtf(
									priestBullet[i].velocityVector.x * priestBullet[i].velocityVector.x +
									priestBullet[i].velocityVector.y * priestBullet[i].velocityVector.y
								);
								if (priestBullet[i].length > 0) {
									priestBullet[i].velocityVectorNew.x = priestBullet[i].velocityVector.x / priestBullet[i].length;
									priestBullet[i].velocityVectorNew.y = priestBullet[i].velocityVector.y / priestBullet[i].length;
									priestBullet[i].Position.x += priestBullet[i].velocityVectorNew.x * priestBullet[i].velocity;
									priestBullet[i].Position.y += priestBullet[i].velocityVectorNew.y * priestBullet[i].velocity;
								}
								else {
									priestBullet[i].isExist = false;
									priestBullet[i].isShoot = false;
								}
							}

							if (/*Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive && Ghost[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin2) {
								priestBullet[i].velocityVector.x = (Ghost[dMin2].Position.x + Ghost[dMin2].width / 2) - (player.Position.x + player.width / 2);
								priestBullet[i].velocityVector.y = (Ghost[dMin2].Position.y + Ghost[dMin2].height / 2) - (player.Position.y + player.height / 2);
								priestBullet[i].length = sqrtf(
									priestBullet[i].velocityVector.x * priestBullet[i].velocityVector.x +
									priestBullet[i].velocityVector.y * priestBullet[i].velocityVector.y
								);

								if (priestBullet[i].length > 0) {
									priestBullet[i].velocityVectorNew.x = priestBullet[i].velocityVector.x / priestBullet[i].length;
									priestBullet[i].velocityVectorNew.y = priestBullet[i].velocityVector.y / priestBullet[i].length;
									priestBullet[i].Position.x += priestBullet[i].velocityVectorNew.x * priestBullet[i].velocity;
									priestBullet[i].Position.y += priestBullet[i].velocityVectorNew.y * priestBullet[i].velocity;
								}
								else {
									priestBullet[i].isExist = false;
									priestBullet[i].isShoot = false;
								}
							}
							if (/*Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive && Demon[i].isMove && */dinstanceMinOfMin == dinstanceMin3) {
								priestBullet[i].velocityVector.x = (Demon[dMin3].Position.x + Demon[dMin3].width / 2) - (player.Position.x + player.width / 2);
								priestBullet[i].velocityVector.y = (Demon[dMin3].Position.y + Demon[dMin3].height / 2) - (player.Position.y + player.height / 2);
								priestBullet[i].length = sqrtf(
									priestBullet[i].velocityVector.x * priestBullet[i].velocityVector.x +
									priestBullet[i].velocityVector.y * priestBullet[i].velocityVector.y
								);

								if (priestBullet[i].length > 0) {
									priestBullet[i].velocityVectorNew.x = priestBullet[i].velocityVector.x / priestBullet[i].length;
									priestBullet[i].velocityVectorNew.y = priestBullet[i].velocityVector.y / priestBullet[i].length;
									priestBullet[i].Position.x += priestBullet[i].velocityVectorNew.x * priestBullet[i].velocity;
									priestBullet[i].Position.y += priestBullet[i].velocityVectorNew.y * priestBullet[i].velocity;
								}
								else {
									priestBullet[i].isExist = false;
									priestBullet[i].isShoot = false;
								}
							}
							if (/*Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive && Dragon[i].isMove &&*/ dinstanceMinOfMin == dinstanceMin4) {
								priestBullet[i].velocityVector.x = (Dragon[dMin4].Position.x + Dragon[dMin4].width / 2) - (player.Position.x + player.width / 2);
								priestBullet[i].velocityVector.y = (Dragon[dMin4].Position.y + Dragon[dMin4].height / 2) - (player.Position.y + player.height / 2);
								priestBullet[i].length = sqrtf(
									priestBullet[i].velocityVector.x * priestBullet[i].velocityVector.x +
									priestBullet[i].velocityVector.y * priestBullet[i].velocityVector.y
								);

								if (priestBullet[i].length > 0) {
									priestBullet[i].velocityVectorNew.x = priestBullet[i].velocityVector.x / priestBullet[i].length;
									priestBullet[i].velocityVectorNew.y = priestBullet[i].velocityVector.y / priestBullet[i].length;
									priestBullet[i].Position.x += priestBullet[i].velocityVectorNew.x * priestBullet[i].velocity;
									priestBullet[i].Position.y += priestBullet[i].velocityVectorNew.y * priestBullet[i].velocity;
								}
								else {
									priestBullet[i].isExist = false;
									priestBullet[i].isShoot = false;
								}
							}
							if (dinstanceMinOfMin == dinstanceMin5) {
								priestBullet[i].velocityVector.x = (Boss.Position.x + Boss.width / 2) - (player.Position.x + player.width / 2);
								priestBullet[i].velocityVector.y = (Boss.Position.y + Boss.height / 2) - (player.Position.y + player.height / 2);
								priestBullet[i].length = sqrtf(
									priestBullet[i].velocityVector.x * priestBullet[i].velocityVector.x +
									priestBullet[i].velocityVector.y * priestBullet[i].velocityVector.y
								);

								if (priestBullet[i].length > 0) {
									priestBullet[i].velocityVectorNew.x = priestBullet[i].velocityVector.x / priestBullet[i].length;
									priestBullet[i].velocityVectorNew.y = priestBullet[i].velocityVector.y / priestBullet[i].length;
									priestBullet[i].Position.x += priestBullet[i].velocityVectorNew.x * priestBullet[i].velocity;
									priestBullet[i].Position.y += priestBullet[i].velocityVectorNew.y * priestBullet[i].velocity;
								}
								else {
									priestBullet[i].isExist = false;
									priestBullet[i].isShoot = false;
								}
							}

						}
						if (priestBullet[i].Position.x < 0 || priestBullet[i].Position.x + priestBullet[i].width > 1280 ||
							priestBullet[i].Position.y < 0 || priestBullet[i].Position.y + priestBullet[i].height > 720 ||
							priestBullet[i].velocityVectorNew.x == 0 && priestBullet[i].velocityVectorNew.y == 0) {
							priestBullet[i].isExist = false;
							priestBullet[i].isShoot = false;
						}
					}
				}

#pragma endregion

#pragma endregion		
				//boss射击
#pragma region boss射击计算 ボスシューティング
				if (Boss.isEnemyAlive && Boss.isEnemyRealAlive) {
					if (Boss.isMove) {
						if (Boss.attackCoolTime > 0) {
							Boss.attackCoolTime--;
						}
						else {
							Boss.attackCoolTime = 300;
						}
						if (Boss.attackCoolTime == 0) {
							for (int i = 0; i < 32; i++) {
								if (bulletBoss[i].isShoot == false) {
									bulletBoss[i].Position.x = (Boss.Position.x + Boss.width / 2);
									bulletBoss[i].Position.y = (Boss.Position.y + Boss.height / 2);
									bulletBoss[i].isShoot = true;
									bulletBoss[i].isExist = true;
								}
							}
						}
						for (int i = 0; i < 32; i++) {
							if (bulletBoss[i].isExist) {
								bulletBoss[i].Position.x += cosf(theta[i]) * bulletBoss[i].velocity;
								bulletBoss[i].Position.y += sinf(theta[i]) * bulletBoss[i].velocity;
							}
							if (reloadTimer > 0) {
								reloadTimer--;
							}
							else {
								reloadTimer = 180;
							}
							if (reloadTimer == 0) {
								bulletBoss[i].isShoot = false;
								bulletBoss[i].isExist = false;
							}
						}
					}
				}

#pragma endregion
#pragma endregion
#pragma region 当たり判定
				//=============================
				//当たり判定
				//=============================
#pragma region 子弹打敌人 当たり判定
			//子弹打敌人
#pragma region goblin
			//goblin
				for (int i = 0; i < 50; i++) {
#pragma region 子弹打goblin 战士子弹特殊效果 短时定身 
					if (Goblin[i].isEnemyRealAlive && Goblin[i].isEnemyAlive) {
						if (Goblin[i].isStop) {
							if (Goblin[i].isStopTimer > 0) {
								Goblin[i].moveVelocity.x = 0;
								Goblin[i].moveVelocity.y = 0;
								Goblin[i].isStopTimer--;
							}
							else {
								Goblin[i].isStop = false;
							}
						}
						else {
							Goblin[i].moveVelocity.x = 2.5f;
							Goblin[i].moveVelocity.y = 2.5f;
							Goblin[i].isStopTimer = 10;
						}
#pragma endregion
						for (int j = 0; j < 50; j++) {
							//战士
							if (warriorBullet[j].isShoot && warriorBullet[j].isExist) {
								if (warriorBullet[j].Position.x <= Goblin[i].Position.x + Goblin[i].width &&
									Goblin[i].Position.x <= warriorBullet[j].Position.x + warriorBullet[j].width &&
									warriorBullet[j].Position.y <= Goblin[i].Position.y + Goblin[i].height &&
									Goblin[i].Position.y <= warriorBullet[j].Position.y + warriorBullet[j].height) {
									warriorBullet[j].isExist = false;
									warriorBullet[j].isShoot = false;
									Goblin[i].isStop = true;
									float damage = DamageCalculation(player.attack, Goblin[i].defense);
									Goblin[i].lifePoint -= damage;
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Goblin[i].lifePoint <= 0) {
										Goblin[i].lifePoint = 0;
										Goblin[i].isMove = false;
										Goblin[i].isEnemyAlive = false;
										moneyNumber += rand() % 3 + 1;
										player.experience += 10;
									}

								}
							}
							//法师
							if (mageBullet[j].isShoot && mageBullet[j].isExist) {
								if (mageBullet[j].Position.x <= Goblin[i].Position.x + Goblin[i].width &&
									Goblin[i].Position.x <= mageBullet[j].Position.x + mageBullet[j].width &&
									mageBullet[j].Position.y <= Goblin[i].Position.y + Goblin[i].height &&
									Goblin[i].Position.y <= mageBullet[j].Position.y + mageBullet[j].height) {
									mageBullet[j].isExist = false;
									mageBullet[j].isShoot = false;
									float damage = DamageCalculation(player.attack, Goblin[i].defense);
									Goblin[i].lifePoint -= damage;
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Goblin[i].lifePoint <= 0) {
										Goblin[i].lifePoint = 0;
										Goblin[i].isMove = false;
										Goblin[i].isEnemyAlive = false;
										moneyNumber += rand() % 3 + 1;
										player.experience += 10;
									}
								}
							}
							//牧师
							if (priestBullet[j].isShoot && priestBullet[j].isExist) {
								if (priestBullet[j].Position.x <= Goblin[i].Position.x + Goblin[i].width &&
									Goblin[i].Position.x <= priestBullet[j].Position.x + priestBullet[j].width &&
									priestBullet[j].Position.y <= Goblin[i].Position.y + Goblin[i].height &&
									Goblin[i].Position.y <= priestBullet[j].Position.y + priestBullet[j].height) {
									priestBullet[j].isExist = false;
									priestBullet[j].isShoot = false;
									rollHealing = rand() % 10;
									if (rollHealing == 6) {
										if (player.lifePoint <= player.lifePointMax - 2) {
											player.lifePoint += 2;
										}
										else {
											player.lifePoint += (player.lifePointMax - player.lifePoint);
										}
									}
									float damage = DamageCalculation(player.attack, Goblin[i].defense);
									Goblin[i].lifePoint -= damage;
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Goblin[i].lifePoint <= 0) {
										Goblin[i].lifePoint = 0;
										Goblin[i].isMove = false;
										Goblin[i].isEnemyAlive = false;
										moneyNumber += rand() % 3 + 1;
										player.experience += 10;
									}
								}
							}
						}
					}
				}
#pragma endregion
#pragma region ghost
				//ghost
				for (int i = 0; i < 50; i++) {
					if (Ghost[i].isEnemyRealAlive && Ghost[i].isEnemyAlive) {
						for (int j = 0; j < 50; j++) {
							//战士
							if (warriorBullet[j].isShoot && warriorBullet[j].isExist) {
								if (warriorBullet[j].Position.x <= Ghost[i].Position.x + Ghost[i].width &&
									Ghost[i].Position.x <= warriorBullet[j].Position.x + warriorBullet[j].width &&
									warriorBullet[j].Position.y <= Ghost[i].Position.y + Ghost[i].height &&
									Ghost[i].Position.y <= warriorBullet[j].Position.y + warriorBullet[j].height) {
									warriorBullet[j].isExist = false;
									warriorBullet[j].isShoot = false;
									Ghost[i].isEnemyImmunity = true;//战士打幽灵免疫，所以无伤害
									/*float damage = DamageCalculation(player.attack, Ghost[i].defense);
									Ghost[i].lifePoint -= damage;
									if (Ghost[i].lifePoint <= 0) {
										Ghost[i].lifePoint = 0;
										Ghost[i].isMove = false;
										Ghost[i].isEnemyAlive = false;
									}*/
								}
							}
							//法师
							if (mageBullet[j].isShoot && mageBullet[j].isExist) {
								if (mageBullet[j].Position.x <= Ghost[i].Position.x + Ghost[i].width &&
									Ghost[i].Position.x <= mageBullet[j].Position.x + mageBullet[j].width &&
									mageBullet[j].Position.y <= Ghost[i].Position.y + Ghost[i].height &&
									Ghost[i].Position.y <= mageBullet[j].Position.y + mageBullet[j].height) {
									mageBullet[j].isExist = false;
									mageBullet[j].isShoot = false;
									Ghost[i].isEnemyImmunity = false;
									float damage = DamageCalculation(player.attack, Ghost[i].defense);
									Ghost[i].lifePoint -= damage * 1.5f;//法师对幽灵造成结算后1.5倍伤害
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Ghost[i].lifePoint <= 0) {
										Ghost[i].lifePoint = 0;
										Ghost[i].isMove = false;
										Ghost[i].isEnemyAlive = false;
										moneyNumber += rand() % 4 + 2;
										player.experience += 25;
									}

								}
							}
							//牧师
							if (priestBullet[j].isShoot && priestBullet[j].isExist) {
								if (priestBullet[j].Position.x <= Ghost[i].Position.x + Ghost[i].width &&
									Ghost[i].Position.x <= priestBullet[j].Position.x + priestBullet[j].width &&
									priestBullet[j].Position.y <= Ghost[i].Position.y + Ghost[i].height &&
									Ghost[i].Position.y <= priestBullet[j].Position.y + priestBullet[j].height) {
									priestBullet[j].isExist = false;
									priestBullet[j].isShoot = false;
									Ghost[i].isEnemyImmunity = false;
									rollHealing = rand() % 10;
									if (rollHealing == 6) {
										if (player.lifePoint <= player.lifePointMax - 2) {
											player.lifePoint += 2;
										}
										else {
											player.lifePoint += (player.lifePointMax - player.lifePoint);
										}
									}
									float damage = DamageCalculation(player.attack, Ghost[i].defense);
									Ghost[i].lifePoint -= damage * 1.2f;//牧师对幽灵造成结算后1.2倍伤害
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Ghost[i].lifePoint <= 0) {
										Ghost[i].lifePoint = 0;
										Ghost[i].isMove = false;
										Ghost[i].isEnemyAlive = false;
										moneyNumber += rand() % 4 + 2;
										player.experience += 25;
									}
								}
							}
						}
					}
				}
#pragma endregion
#pragma region demon
				//demon
				for (int i = 0; i < 50; i++) {
#pragma region 子弹打demon 战士子弹特殊效果 短时定身 
					if (Demon[i].isEnemyRealAlive && Demon[i].isEnemyAlive) {
						if (Demon[i].isStop) {
							if (Demon[i].isStopTimer > 0) {
								Demon[i].moveVelocity.x = 0;
								Demon[i].moveVelocity.y = 0;
								Demon[i].isStopTimer--;
							}
							else {
								Demon[i].isStop = false;
							}
						}
						else {
							Demon[i].moveVelocity.x = 2.0f;
							Demon[i].moveVelocity.y = 2.0f;
							Demon[i].isStopTimer = 10;
						}
#pragma endregion
						for (int j = 0; j < 50; j++) {
							//战士
							if (warriorBullet[j].isShoot && warriorBullet[j].isExist) {
								if (warriorBullet[j].Position.x <= Demon[i].Position.x + Demon[i].width &&
									Demon[i].Position.x <= warriorBullet[j].Position.x + warriorBullet[j].width &&
									warriorBullet[j].Position.y <= Demon[i].Position.y + Demon[i].height &&
									Demon[i].Position.y <= warriorBullet[j].Position.y + warriorBullet[j].height) {
									warriorBullet[j].isExist = false;
									warriorBullet[j].isShoot = false;
									Demon[i].isEnemyImmunity = false;
									Demon[i].isStop = true;
									float damage = DamageCalculation(player.attack, Demon[i].defense);
									Demon[i].lifePoint -= damage * 1.2f;
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Demon[i].lifePoint <= 0) {
										Demon[i].lifePoint = 0;
										Demon[i].isMove = false;
										Demon[i].isEnemyAlive = false;
										moneyNumber += rand() % 7 + 3;
										player.experience += 50;
									}
								}
							}
							//法师
							if (mageBullet[j].isShoot && mageBullet[j].isExist) {
								if (mageBullet[j].Position.x <= Demon[i].Position.x + Demon[i].width &&
									Demon[i].Position.x <= mageBullet[j].Position.x + mageBullet[j].width &&
									mageBullet[j].Position.y <= Demon[i].Position.y + Demon[i].height &&
									Demon[i].Position.y <= mageBullet[j].Position.y + mageBullet[j].height) {
									mageBullet[j].isExist = false;
									mageBullet[j].isShoot = false;
									Demon[i].isEnemyImmunity = false;
									float damage = DamageCalculation(player.attack, Demon[i].defense);
									Demon[i].lifePoint -= damage * 0.8f;//恶魔抵抗魔法攻击
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Demon[i].lifePoint <= 0) {
										Demon[i].lifePoint = 0;
										Demon[i].isMove = false;
										Demon[i].isEnemyAlive = false;
										moneyNumber += rand() % 7 + 3;
										player.experience += 50;
									}
								}
							}
							//牧师
							if (priestBullet[j].isShoot && priestBullet[j].isExist) {
								if (priestBullet[j].Position.x <= Demon[i].Position.x + Demon[i].width &&
									Demon[i].Position.x <= priestBullet[j].Position.x + priestBullet[j].width &&
									priestBullet[j].Position.y <= Demon[i].Position.y + Demon[i].height &&
									Demon[i].Position.y <= priestBullet[j].Position.y + priestBullet[j].height) {
									priestBullet[j].isExist = false;
									priestBullet[j].isShoot = false;
									Demon[i].isEnemyImmunity = false;
									rollHealing = rand() % 10;
									if (rollHealing == 6) {
										if (player.lifePoint <= player.lifePointMax - 2) {
											player.lifePoint += 2;
										}
										else {
											player.lifePoint += (player.lifePointMax - player.lifePoint);
										}
									}
									float damage = DamageCalculation(player.attack, Demon[i].defense);
									Demon[i].lifePoint -= damage * 2.0f;//牧师对恶魔造成双倍伤害
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Demon[i].lifePoint <= 0) {
										Demon[i].lifePoint = 0;
										Demon[i].isMove = false;
										Demon[i].isEnemyAlive = false;
										moneyNumber += rand() % 7 + 3;
										player.experience += 50;
									}
								}
							}
						}
					}
				}
#pragma endregion
#pragma region dragon
				//dragon
				for (int i = 0; i < 50; i++) {
#pragma region 子弹打dragon 战士子弹特殊效果 短时定身 
					if (Dragon[i].isEnemyRealAlive && Dragon[i].isEnemyAlive) {
						if (Dragon[i].isStop) {
							if (Dragon[i].isStopTimer > 0) {
								Dragon[i].moveVelocity.x = 0;
								Dragon[i].moveVelocity.y = 0;
								Dragon[i].isStopTimer--;
							}
							else {
								Dragon[i].isStop = false;
							}
						}
						else {
							Dragon[i].moveVelocity.x = 1.5f;
							Dragon[i].moveVelocity.y = 1.5f;
							Dragon[i].isStopTimer = 10;
						}
#pragma endregion
						for (int j = 0; j < 50; j++) {
							//战士
							if (warriorBullet[j].isShoot && warriorBullet[j].isExist) {
								if (warriorBullet[j].Position.x <= Dragon[i].Position.x + Dragon[i].width &&
									Dragon[i].Position.x <= warriorBullet[j].Position.x + warriorBullet[j].width &&
									warriorBullet[j].Position.y <= Dragon[i].Position.y + Dragon[i].height &&
									Dragon[i].Position.y <= warriorBullet[j].Position.y + warriorBullet[j].height) {
									warriorBullet[j].isExist = false;
									warriorBullet[j].isShoot = false;
									Dragon[i].isEnemyImmunity = false;
									Dragon[i].isStop = true;
									float damage = DamageCalculation(player.attack, Dragon[i].defense);
									Dragon[i].lifePoint -= damage * 1.5f;//战士对龙造成1.5倍伤害
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Dragon[i].lifePoint <= 0) {
										Dragon[i].lifePoint = 0;
										Dragon[i].isMove = false;
										Dragon[i].isEnemyAlive = false;
										moneyNumber += rand() % 6 + 10;
										player.experience += 100;
									}
								}
							}
							//法师
							if (mageBullet[j].isShoot && mageBullet[j].isExist) {
								if (mageBullet[j].Position.x <= Dragon[i].Position.x + Dragon[i].width &&
									Dragon[i].Position.x <= mageBullet[j].Position.x + mageBullet[j].width &&
									mageBullet[j].Position.y <= Dragon[i].Position.y + Dragon[i].height &&
									Dragon[i].Position.y <= mageBullet[j].Position.y + mageBullet[j].height) {
									mageBullet[j].isExist = false;
									mageBullet[j].isShoot = false;
									Dragon[i].isEnemyImmunity = true;//龙免疫魔法
									/*float damage = DamageCalculation(player.attack, Dragon[i].defense);
									Dragon[i].lifePoint -= damage;
									if (Dragon[i].lifePoint <= 0) {
										Dragon[i].lifePoint = 0;
										Dragon[i].isMove = false;
										Dragon[i].isEnemyAlive = false;
									}*/
								}
							}
							//牧师
							if (priestBullet[j].isShoot && priestBullet[j].isExist) {
								if (priestBullet[j].Position.x <= Dragon[i].Position.x + Dragon[i].width &&
									Dragon[i].Position.x <= priestBullet[j].Position.x + priestBullet[j].width &&
									priestBullet[j].Position.y <= Dragon[i].Position.y + Dragon[i].height &&
									Dragon[i].Position.y <= priestBullet[j].Position.y + priestBullet[j].height) {
									priestBullet[j].isExist = false;
									priestBullet[j].isShoot = false;
									Dragon[i].isEnemyImmunity = false;
									rollHealing = rand() % 10;
									if (rollHealing == 6) {
										if (player.lifePoint <= player.lifePointMax - 2) {
											player.lifePoint += 2;
										}
										else {
											player.lifePoint += (player.lifePointMax - player.lifePoint);
										}
									}
									float damage = DamageCalculation(player.attack, Dragon[i].defense);
									Dragon[i].lifePoint -= damage * 0.8f;//龙抵抗神圣 造成0.8倍伤害
									voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
									if (Dragon[i].lifePoint <= 0) {
										Dragon[i].lifePoint = 0;
										Dragon[i].isMove = false;
										Dragon[i].isEnemyAlive = false;
										moneyNumber += rand() % 6 + 10;
										player.experience += 100;
									}
								}
							}
						}
					}
				}
#pragma endregion
#pragma region boss
				for (int j = 0; j < 50; j++) {
					//战士
					if (warriorBullet[j].isShoot && warriorBullet[j].isExist) {
						if (warriorBullet[j].Position.x <= Boss.Position.x + Boss.width &&
							Boss.Position.x <= warriorBullet[j].Position.x + warriorBullet[j].width &&
							warriorBullet[j].Position.y <= Boss.Position.y + Boss.height &&
							Boss.Position.y <= warriorBullet[j].Position.y + warriorBullet[j].height) {
							warriorBullet[j].isExist = false;
							warriorBullet[j].isShoot = false;
							float damage = DamageCalculation(player.attack, Boss.defense);
							Boss.lifePoint -= damage;
							voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
							if (Boss.lifePoint <= 0) {
								Boss.lifePoint = 0;
								Boss.isMove = false;
								Boss.isEnemyAlive = false;
								Boss.isEnemyRealAlive = false;
								scene = END2;
							}

						}
					}
					//法师
					if (mageBullet[j].isShoot && mageBullet[j].isExist) {
						if (mageBullet[j].Position.x <= Boss.Position.x + Boss.width &&
							Boss.Position.x <= mageBullet[j].Position.x + mageBullet[j].width &&
							mageBullet[j].Position.y <= Boss.Position.y + Boss.height &&
							Boss.Position.y <= mageBullet[j].Position.y + mageBullet[j].height) {
							mageBullet[j].isExist = false;
							mageBullet[j].isShoot = false;
							float damage = DamageCalculation(player.attack, Boss.defense);
							Boss.lifePoint -= damage;
							voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
							if (Boss.lifePoint <= 0) {
								Boss.lifePoint = 0;
								Boss.isMove = false;
								Boss.isEnemyAlive = false;
								Boss.isEnemyRealAlive = false;
								scene = END2;
							}
						}
					}
					//牧师
					if (priestBullet[j].isShoot && priestBullet[j].isExist) {
						if (priestBullet[j].Position.x <= Boss.Position.x + Boss.width &&
							Boss.Position.x <= priestBullet[j].Position.x + priestBullet[j].width &&
							priestBullet[j].Position.y <= Boss.Position.y + Boss.height &&
							Boss.Position.y <= priestBullet[j].Position.y + priestBullet[j].height) {
							priestBullet[j].isExist = false;
							priestBullet[j].isShoot = false;
							rollHealing = rand() % 10;
							if (rollHealing == 6) {
								if (player.lifePoint <= player.lifePointMax - 2) {
									player.lifePoint += 2;
								}
								else {
									player.lifePoint += (player.lifePointMax - player.lifePoint);
								}
							}
							float damage = DamageCalculation(player.attack, Boss.defense);
							Boss.lifePoint -= damage;
							voiceHandle_Hit = Novice::PlayAudio(SOUND_HIT, false, 0.3f);
							if (Boss.lifePoint <= 0) {
								Boss.lifePoint = 0;
								Boss.isMove = false;
								Boss.isEnemyAlive = false;
								Boss.isEnemyRealAlive = false;
								scene = END2;
							}
						}
					}
				}
#pragma endregion
#pragma endregion
#pragma region 敌人打自己 当たり判定
				//无敌时间
				if (player.isInvincible) {
					player.invincibleTimer--;
					if (player.invincibleTimer < 0) {
						player.invincibleTimer = 0;
					}
					if (player.invincibleTimer == 0) {
						player.isInvincible = false;
					}
				}
				//goblin
#pragma region goblin
				for (int i = 0; i < 50; i++) {
					if (Goblin[i].isEnemyAlive && Goblin[i].isEnemyRealAlive && Goblin[i].isMove) {
						if (Goblin[i].attackCoolTime > 0) {
							Goblin[i].attackCoolTime--;
						}
						if (Goblin[i].attackCoolTime < 0) {
							Goblin[i].attackCoolTime = 0;
						}
						if (Goblin[i].attackCoolTime == 0) {
							if (Goblin[i].Position.x <= player.Position.x + player.width &&
								player.Position.x <= Goblin[i].Position.x + Goblin[i].width &&
								Goblin[i].Position.y <= player.Position.y + player.height &&
								player.Position.y <= Goblin[i].Position.y + Goblin[i].height) {
								if (!player.isInvincible) {
									float damage = DamageCalculation(Goblin[i].attack, player.defense);
									player.lifePoint -= damage;
									Goblin[i].attackCoolTime = 60;
									player.isInvincible = true;
									player.invincibleTimer = 30;
									if (player.lifePoint <= 0) {
										scene = DEATH;
									}
								}
								else {
									player.lifePoint -= 0;
									Goblin[i].attackCoolTime = 60;
								}
							}
						}
					}
				}
#pragma endregion
				//ghost
#pragma region ghost
				for (int i = 0; i < 50; i++) {
					if (Ghost[i].isEnemyAlive && Ghost[i].isEnemyRealAlive && Ghost[i].isMove) {
						if (Ghost[i].attackCoolTime > 0) {
							Ghost[i].attackCoolTime--;
						}
						if (Ghost[i].attackCoolTime < 0) {
							Ghost[i].attackCoolTime = 0;
						}
						if (Ghost[i].attackCoolTime == 0) {
							if (Ghost[i].Position.x <= player.Position.x + player.width &&
								player.Position.x <= Ghost[i].Position.x + Ghost[i].width &&
								Ghost[i].Position.y <= player.Position.y + player.height &&
								player.Position.y <= Ghost[i].Position.y + Ghost[i].height) {
								if (!player.isInvincible) {
									float damage = DamageCalculation(Ghost[i].attack, player.defense);
									player.lifePoint -= damage;
									Ghost[i].attackCoolTime = 60;
									player.isInvincible = true;
									player.invincibleTimer = 30;
									if (player.lifePoint <= 0) {
										scene = DEATH;
									}
								}
								else {
									player.lifePoint -= 0;
									Ghost[i].attackCoolTime = 60;
								}
							}
						}
					}
				}

#pragma endregion
				//demon
#pragma region demon
				for (int i = 0; i < 50; i++) {
					if (Demon[i].isEnemyAlive && Demon[i].isEnemyRealAlive && Demon[i].isMove) {
						if (Demon[i].attackCoolTime > 0) {
							Demon[i].attackCoolTime--;
						}
						if (Demon[i].attackCoolTime < 0) {
							Demon[i].attackCoolTime = 0;
						}
						if (Demon[i].attackCoolTime == 0) {
							if (Demon[i].Position.x <= player.Position.x + player.width &&
								player.Position.x <= Demon[i].Position.x + Demon[i].width &&
								Demon[i].Position.y <= player.Position.y + player.height &&
								player.Position.y <= Demon[i].Position.y + Demon[i].height) {
								if (!player.isInvincible) {
									float damage = DamageCalculation(Demon[i].attack, player.defense);
									player.lifePoint -= damage;
									Demon[i].attackCoolTime = 60;
									player.isInvincible = true;
									player.invincibleTimer = 30;
									if (player.lifePoint <= 0) {
										scene = DEATH;
									}
								}
								else {
									player.lifePoint -= 0;
									Demon[i].attackCoolTime = 60;
								}
							}
						}
					}
				}

#pragma endregion
				//dragon
#pragma region dragon
				for (int i = 0; i < 50; i++) {
					if (Dragon[i].isEnemyAlive && Dragon[i].isEnemyRealAlive && Dragon[i].isMove) {
						if (Dragon[i].attackCoolTime > 0) {
							Dragon[i].attackCoolTime--;
						}
						if (Dragon[i].attackCoolTime < 0) {
							Dragon[i].attackCoolTime = 0;
						}
						if (Dragon[i].attackCoolTime == 0) {
							if (Dragon[i].Position.x <= player.Position.x + player.width &&
								player.Position.x <= Dragon[i].Position.x + Dragon[i].width &&
								Dragon[i].Position.y <= player.Position.y + player.height &&
								player.Position.y <= Dragon[i].Position.y + Dragon[i].height) {
								if (!player.isInvincible) {
									float damage = DamageCalculation(Dragon[i].attack, player.defense);
									player.lifePoint -= damage;
									Dragon[i].attackCoolTime = 60;
									player.isInvincible = true;
									player.invincibleTimer = 30;
									if (player.lifePoint <= 0) {
										scene = DEATH;
									}
								}
								else {
									player.lifePoint -= 0;
									Dragon[i].attackCoolTime = 60;
								}
							}
						}
					}
				}

#pragma endregion
				//boss
#pragma region boss
				//本体
				if (Boss.isEnemyAlive && Boss.isEnemyRealAlive && Boss.isMove) {
					if (Boss.attackCoolTime > 0) {
						Boss.attackCoolTime--;
					}
					if (Boss.attackCoolTime < 0) {
						Boss.attackCoolTime = 0;
					}
					if (Boss.attackCoolTime == 0) {
						if (Boss.Position.x <= player.Position.x + player.width &&
							player.Position.x <= Boss.Position.x + Boss.width &&
							Boss.Position.y <= player.Position.y + player.height &&
							player.Position.y <= Boss.Position.y + Boss.height) {
							if (!player.isInvincible) {
								float damage = DamageCalculation(Boss.attack, player.defense);
								player.lifePoint -= damage;
								Boss.attackCoolTime = 60;
								player.isInvincible = true;
								player.invincibleTimer = 30;
								if (player.lifePoint <= 0) {
									scene = END1;
								}
							}
							else {
								player.lifePoint -= 0;
								Boss.attackCoolTime = 60;
							}
						}
					}
				}
				//子弹
				for (int i = 0; i < 32; i++) {
					if (bulletBoss[i].isExist) {
						if (bulletBoss[i].Position.x <= player.Position.x + player.width &&
							player.Position.x <= bulletBoss[i].Position.x + bulletBoss[i].width &&
							bulletBoss[i].Position.y <= player.Position.y + player.height &&
							player.Position.y <= bulletBoss[i].Position.y + bulletBoss[i].height) {
							if (!player.isInvincible) {
								float damage = DamageCalculation(Boss.attack, player.defense);
								player.lifePoint -= damage;
								player.isInvincible = true;
								player.invincibleTimer = 30;
								bulletBoss[i].isExist = false;
								if (player.lifePoint <= 0) {
									scene = END1;
								}
							}
							else {
								player.lifePoint -= 0;
								bulletBoss[i].isExist = false;
							}
						}
					}
				}
#pragma endregion

#pragma endregion
#pragma endregion
			}
#pragma region 游戏时间结束 跳转到loading界面  
			if (gameTimer == 0) {
				for (int i = 0; i < 50; i++) {
					Goblin[i].isEnemyRealAlive = false;
					Ghost[i].isEnemyRealAlive = false;
					Demon[i].isEnemyRealAlive = false;
					Dragon[i].isEnemyRealAlive = false;
					Goblin[i].isEnemyAlive = false;
					Ghost[i].isEnemyAlive = false;
					Demon[i].isEnemyAlive = false;
					Dragon[i].isEnemyAlive = false;
				}
				isStagePassed = true;
				if (scene == STAGE1 || scene == STAGE2) {
					if (isStagePassed) {
						if (scene == STAGE1) {
							scene = LOADING;
							lastStage = STAGE1;
						}
						else if (scene == STAGE2) {
							scene = LOADING;
							lastStage = STAGE2;
						}
						isBuffRandom = false;
					}
				}
				else if (scene == BOSS) {
					if (isStagePassed && Boss.isEnemyAlive) {//BE
						scene = END1;
						isBuffRandom = false;
					}
					else if (isStagePassed && !Boss.isEnemyAlive) {//HE
						scene = END2;
						isBuffRandom = false;
					}
				}
			}
#pragma endregion
		}
#pragma region loading跳转到奖励界面
		else if ((lastStage == STAGE1 || lastStage == STAGE2) && scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				scene = STAGE_PASSED_REWARD;
				SCENE_CHANGE_TIMER = 60;
			}
		}
#pragma endregion
#pragma endregion
#pragma region 奖励界面和buff数值  リワード

		if (scene == STAGE_PASSED_REWARD) {
			Novice::StopAudio(voiceHandle_StageStart);
			Novice::StopAudio(voiceHandle_Stage1);
			Novice::StopAudio(voiceHandle_Stage2);
			Novice::StopAudio(voiceHandle_StageBoss);
#pragma region 随机buff值
			if (isBuffRandom == false) {
				int randomIndex1 = rand() % 4 + 1;
				int randomIndex2 = rand() % 4 + 1;
				int randomIndex3 = rand() % 4 + 1;
				buff[1] = buffMap[randomIndex1];
				buff[2] = buffMap[randomIndex2];
				buff[3] = buffMap[randomIndex3];
				isBuffRandom = true;
			}
#pragma endregion
#pragma region 选项间切换
			if (RewardSelection == REWARD_1) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					RewardSelection = REWARD_2;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (RewardSelection == REWARD_2) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					RewardSelection = REWARD_3;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			if (RewardSelection == REWARD_2) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					RewardSelection = REWARD_1;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (RewardSelection == REWARD_3) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					RewardSelection = REWARD_2;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
#pragma endregion
#pragma region 选项确定和切换到loading界面
			if (RewardSelection == REWARD_1 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0) {
				buff[0] = buff[1];
				isItemRandom = false;
				voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
				scene = LOADING;
				preScene = STAGE_PASSED_REWARD;
			}
			if (RewardSelection == REWARD_2 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0) {
				buff[0] = buff[2];
				isItemRandom = false;
				voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
				scene = LOADING;
				preScene = STAGE_PASSED_REWARD;
			}
			if (RewardSelection == REWARD_3 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0) {
				buff[0] = buff[3];
				isItemRandom = false;
				voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
				scene = LOADING;
				preScene = STAGE_PASSED_REWARD;
			}

#pragma endregion
#pragma region buff属性增加

			if (buff[0] == speedBuff) {
				player.moveVelocity.x += 1;
				player.moveVelocity.y += 1;
				player.moveVelocityMax.x += 1;
				player.moveVelocityMax.y += 1;
				buff[0] = nothing_buff;

			}
			if (buff[0] == lifeBuff) {
				player.lifePointMax += 5;
				buff[0] = nothing_buff;
			}
			if (buff[0] == attackBuff) {
				player.attack += 1;
				buff[0] = nothing_buff;
			}
			if (buff[0] == defenseBuff) {
				player.defense += 1;
				buff[0] = nothing_buff;
			}
			for (int i = 1; i < 4; i++) {
				if (buff[i] == speedBuff) {
					TEXTURE_BUFF_ON[i] = TEXTURE_BUFF_SPD;
				}
				if (buff[i] == attackBuff) {
					TEXTURE_BUFF_ON[i] = TEXTURE_BUFF_ATK;
				}
				if (buff[i] == defenseBuff) {
					TEXTURE_BUFF_ON[i] = TEXTURE_BUFF_DEF;
				}
				if (buff[i] == lifeBuff) {
					TEXTURE_BUFF_ON[i] = TEXTURE_BUFF_LIF;
				}
			}


#pragma endregion
		}
#pragma region loading界面 切换至shop
		else if (scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				RewardSelection = REWARD_1;
				scene = SHOP;
				SCENE_CHANGE_TIMER = 60;
			}
		}
#pragma endregion
#pragma endregion
#pragma region 商店界面 ショップ
		if (scene == SHOP) {
			Novice::StopAudio(voiceHandle_StageStart);
			Novice::StopAudio(voiceHandle_Stage1);
			Novice::StopAudio(voiceHandle_Stage2);
			Novice::StopAudio(voiceHandle_StageBoss);
#pragma region 随机物品的值
			if (isItemRandom == false) {
				int randomItemIndex1 = rand() % 6 + 1;
				int randomItemIndex2 = rand() % 6 + 1;
				int randomItemIndex3 = rand() % 6 + 1;
				Itemlist[1] = ItemlistMap[randomItemIndex1];
				Itemlist[2] = ItemlistMap[randomItemIndex2];
				Itemlist[3] = ItemlistMap[randomItemIndex3];
				isItemRandom = true;
			}
#pragma endregion
#pragma region 商店界面选项切换
			if (ShopSelection == ITEM_1) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					ShopSelection = ITEM_2;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (ShopSelection == ITEM_2) {
				if (keys[DIK_D] && preKeys[DIK_D] == 0 || keys[DIK_RIGHT] && preKeys[DIK_RIGHT] == 0) {
					ShopSelection = ITEM_3;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			if (ShopSelection == ITEM_2) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					ShopSelection = ITEM_1;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
			else if (ShopSelection == ITEM_3) {
				if (keys[DIK_A] && preKeys[DIK_A] == 0 || keys[DIK_LEFT] && preKeys[DIK_LEFT] == 0) {
					ShopSelection = ITEM_2;
					voiceHandle_Select = Novice::PlayAudio(SOUND_SELECT, false, 0.5f);
				}
			}
#pragma endregion
#pragma region 商店界面选项确定 并跳转到loading
			if (ShopSelection == ITEM_1 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0 && isBought1 == false) {
				Itemlist[0] = Itemlist[1];
				for (int i = 0; i < 7; i++) {
					if (Item_Select[i].name == Itemlist[0]) {
						if (moneyNumber >= Item_Select[i].cost) {
							moneyNumber = moneyNumber - Item_Select[i].cost;
							//scene = LOADING;
							//preScene = SHOP;
							isSuccess = true;
							isBought1 = true;
							voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
						}
					}
				}

			}
			if (ShopSelection == ITEM_2 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0 && isBought2 == false) {
				Itemlist[0] = Itemlist[2];
				for (int i = 0; i < 7; i++) {
					if (Item_Select[i].name == Itemlist[0]) {
						if (moneyNumber >= Item_Select[i].cost) {
							moneyNumber = moneyNumber - Item_Select[i].cost;
							//scene = LOADING;
							//preScene = SHOP;
							isSuccess = true;
							isBought2 = true;
							voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
						}
					}
				}
			}
			if (ShopSelection == ITEM_3 && keys[DIK_RETURN] && preKeys[DIK_RETURN] == 0 && isBought3 == false) {
				Itemlist[0] = Itemlist[3];
				for (int i = 0; i < 7; i++) {
					if (Item_Select[i].name == Itemlist[0]) {
						if (moneyNumber >= Item_Select[i].cost) {
							moneyNumber = moneyNumber - Item_Select[i].cost;
							//scene = LOADING;
							//preScene = SHOP;
							isSuccess = true;
							isBought3 = true;
							voiceHandle_Confirm2 = Novice::PlayAudio(SOUND_CONFIRM2, false, 0.5f);
						}
					}
				}
			}
			if (keys[DIK_BACKSPACE] && preKeys[DIK_BACKSPACE] == 0) {
				scene = LOADING;
				preScene = SHOP;
			}
#pragma endregion
#pragma region 物品属性增加
			if (Itemlist[0] == SpeedBoots && isSuccess == true) {
				player.moveVelocity.x += Item_Select[1].speed.x;
				player.moveVelocity.y += Item_Select[1].speed.x;
				player.moveVelocityMax.x += Item_Select[1].speed.x;
				player.moveVelocityMax.y += Item_Select[1].speed.y;
				player.lifePointMax += Item_Select[1].lifePointMax;
				player.attack += Item_Select[1].attack;
				player.defense += Item_Select[1].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			if (Itemlist[0] == LifeNecklace && isSuccess == true) {
				player.moveVelocity.x += Item_Select[2].speed.x;
				player.moveVelocity.y += Item_Select[2].speed.x;
				player.moveVelocityMax.x += Item_Select[2].speed.x;
				player.moveVelocityMax.y += Item_Select[2].speed.y;
				player.lifePointMax += Item_Select[2].lifePointMax;
				player.attack += Item_Select[2].attack;
				player.defense += Item_Select[2].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			if (Itemlist[0] == PowerRing && isSuccess == true) {
				player.moveVelocity.x += Item_Select[3].speed.x;
				player.moveVelocity.y += Item_Select[3].speed.x;
				player.moveVelocityMax.x += Item_Select[3].speed.x;
				player.moveVelocityMax.y += Item_Select[3].speed.y;
				player.lifePointMax += Item_Select[3].lifePointMax;
				player.attack += Item_Select[3].attack;
				player.defense += Item_Select[3].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			if (Itemlist[0] == Buckler && isSuccess == true) {
				player.moveVelocity.x += Item_Select[4].speed.x;
				player.moveVelocity.y += Item_Select[4].speed.x;
				player.moveVelocityMax.x += Item_Select[4].speed.x;
				player.moveVelocityMax.y += Item_Select[4].speed.y;
				player.lifePointMax += Item_Select[4].lifePointMax;
				player.attack += Item_Select[4].attack;
				player.defense += Item_Select[4].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			if (Itemlist[0] == HolyEmblem && isSuccess == true) {
				player.moveVelocity.x += Item_Select[5].speed.x;
				player.moveVelocity.y += Item_Select[5].speed.x;
				player.moveVelocityMax.x += Item_Select[5].speed.x;
				player.moveVelocityMax.y += Item_Select[5].speed.y;
				player.lifePointMax += Item_Select[5].lifePointMax;
				player.attack += Item_Select[5].attack;
				player.defense += Item_Select[5].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			if (Itemlist[0] == DragonSlayingSword && isSuccess == true) {
				player.moveVelocity.x += Item_Select[6].speed.x;
				player.moveVelocity.y += Item_Select[6].speed.x;
				player.moveVelocityMax.x += Item_Select[6].speed.x;
				player.moveVelocityMax.y += Item_Select[6].speed.y;
				player.lifePointMax += Item_Select[6].lifePointMax;
				player.attack += Item_Select[6].attack;
				player.defense += Item_Select[6].defense;
				Itemlist[0] = nothing_item;
				isSuccess = false;
			}
			for (int i = 1; i < 4; i++) {
				if (Itemlist[i] == SpeedBoots) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_SPEEDBOOTS;
				}
				if (Itemlist[i] == LifeNecklace) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_LIFENECKLACE;
				}
				if (Itemlist[i] == PowerRing) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_POWERRING;
				}
				if (Itemlist[i] == Buckler) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_BUCKLER;
				}
				if (Itemlist[i] == HolyEmblem) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_HOLYEMBLEM;
				}
				if (Itemlist[i] == DragonSlayingSword) {
					TEXTURE_ITEM_ON[i] = TEXTURE_ITEM_DSSWORD;
				}
			}
#pragma endregion
		}
#pragma region loading界面 切换至stage2或boss
		else if (lastStage == STAGE1 && preScene == SHOP && scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				player.Position.x = 640.0f;
				player.Position.x = 360.0f;
				player.lifePoint = player.lifePointMax;
				gameTimer = timerValue;
				for (int i = 0; i < 50; i++) {
					Goblin[i].isEnemyRealAlive = true;
					Ghost[i].isEnemyRealAlive = true;
					Demon[i].isEnemyRealAlive = true;
					Dragon[i].isEnemyRealAlive = true;
					Goblin[i].isEnemyAlive = false;
					Ghost[i].isEnemyAlive = false;
					Demon[i].isEnemyAlive = false;
					Dragon[i].isEnemyAlive = false;
					Goblin[i].isMove = false;
					Ghost[i].isMove = false;
					Demon[i].isMove = false;
					Dragon[i].isMove = false;
					Goblin[i].respawnTimer = respawnTimerValue - 30;
					Ghost[i].respawnTimer = respawnTimerValue - 30;
					Demon[i].respawnTimer = respawnTimerValue - 30;
					Dragon[i].respawnTimer = respawnTimerValue - 30;

				}
				isStagePassed = false;
				ShopSelection = ITEM_1;
				isBought1 = false;
				isBought2 = false;
				isBought3 = false;
				isOnRiver = false;
				scene = STAGE2;
				SCENE_CHANGE_TIMER = 60;
			}
		}
		else if (lastStage == STAGE2 && preScene == SHOP && scene == LOADING) {
			SCENE_CHANGE_TIMER--;
			if (SCENE_CHANGE_TIMER <= 0) {
				player.Position.x = 640.0f;
				player.Position.x = 360.0f;
				player.lifePoint = player.lifePointMax;
				gameTimer = timerValue;
				isStagePassed = false;
				isOnRiver = false;
				player.moveVelocityMax.x = player.moveVelocity.x;
				player.moveVelocityMax.y = player.moveVelocity.y;
				scene = BOSS;
				SCENE_CHANGE_TIMER = 60;
			}
		}
#pragma endregion
#pragma endregion
		if (!isStagePassed && scene == STAGE2) {
#pragma region 渡河减速 川をこえる 減速
			mapPosition.x = player.Position.x / size;
			mapPosition.y = player.Position.y / size;
			if (map[(int)mapPosition.y][(int)mapPosition.x] == 1) {
				isOnRiver = true;
				isOnRiverTimer = 60;
			}
			else {
				if (isOnRiverTimer > 0) {
					isOnRiverTimer--;
				}
				else {
					isOnRiverTimer = 0;
				}
				if (isOnRiverTimer == 0) {
					isOnRiver = false;
				}
			}
			if (isOnRiver) {
				player.moveVelocityMax.x = player.moveVelocity.x * 0.6f;
				player.moveVelocityMax.y = player.moveVelocity.y * 0.6f;
			}
			else {
				player.moveVelocityMax.x = player.moveVelocity.x;
				player.moveVelocityMax.y = player.moveVelocity.y;
			}
#pragma endregion
		}
		if (scene == END1 || scene == END2 || scene == DEATH) {
			Novice::StopAudio(voiceHandle_StageStart);
			Novice::StopAudio(voiceHandle_Stage1);
			Novice::StopAudio(voiceHandle_Stage2);
			Novice::StopAudio(voiceHandle_StageBoss);
#pragma region  重置并返回start  リセット
			if (keys[DIK_BACKSPACE] && preKeys[DIK_BACKSPACE] == 0) {
				gameTimer = timerValue;
				for (int i = 0; i < 50; i++) {
					Goblin[i].Position.x = -100.0f;                //world position
					Goblin[i].Position.y = -100.0f;                //world position
					Goblin[i].moveVelocity.x = 2.5f;               //移动速度
					Goblin[i].moveVelocity.y = 2.5f;               //移动速度
					Goblin[i].width = 64;                          //宽
					Goblin[i].height = 64;                         //高
					Goblin[i].attribute = GOBLIN_NORMAL;           //属性
					Goblin[i].lifePoint = 40;                      //生命值
					Goblin[i].lifePointMax = 40;                   //生命值
					Goblin[i].attack = 3;                          //攻击力
					Goblin[i].defense = 0;                         //防御力
					Goblin[i].isEnemyAlive = false;                //生存flag 
					Goblin[i].isEnemyRealAlive = true;             //生存flag2
					Goblin[i].isMove = false;                      //移动flag
					Goblin[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
					Goblin[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
					Goblin[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
					Goblin[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
					Goblin[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
					Goblin[i].respawnTimer = respawnTimerValue;    //复活计时器
					Goblin[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
					Goblin[i].attackCoolTime = 60;//攻击计时器
					Goblin[i].isEnemyImmunity = false;
					Goblin[i].isStop = false;
					Goblin[i].isStopTimer = 10;

					Ghost[i].Position.x = -100.0f;                //world position
					Ghost[i].Position.y = -100.0f;                //world position
					Ghost[i].moveVelocity.x = 4.0f;               //移动速度
					Ghost[i].moveVelocity.y = 4.0f;               //移动速度
					Ghost[i].width = 64;
					Ghost[i].height = 64;
					Ghost[i].attribute = GHOST_VOID;              //属性
					Ghost[i].lifePoint = 25;                      //生命值
					Ghost[i].lifePointMax = 25;                   //生命值
					Ghost[i].attack = 5;                          //攻击力
					Ghost[i].defense = 1;                         //防御力
					Ghost[i].isEnemyAlive = false;                //生存flag 
					Ghost[i].isEnemyRealAlive = true;             //生存flag2
					Ghost[i].isMove = false;                      //移动flag
					Ghost[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
					Ghost[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
					Ghost[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
					Ghost[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
					Ghost[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
					Ghost[i].respawnTimer = respawnTimerValue;    //复活计时器
					Ghost[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
					Ghost[i].attackCoolTime = 60;
					Ghost[i].isEnemyImmunity = false;
					Ghost[i].isStop = false;
					Ghost[i].isStopTimer = 10;

					Demon[i].Position.x = -100.0f;                //world position
					Demon[i].Position.y = -100.0f;                //world position
					Demon[i].moveVelocity.x = 2.0f;               //移动速度
					Demon[i].moveVelocity.y = 2.0f;               //移动速度
					Demon[i].width = 64;
					Demon[i].height = 64;
					Demon[i].attribute = DEMON_EVIL;              //属性
					Demon[i].lifePoint = 60;                      //生命值
					Demon[i].lifePointMax = 60;                   //生命值
					Demon[i].attack = 6;                          //攻击力
					Demon[i].defense = 2;                         //防御力
					Demon[i].isEnemyAlive = false;                //生存flag 
					Demon[i].isEnemyRealAlive = true;             //生存flag2
					Demon[i].isMove = false;                      //移动flag
					Demon[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
					Demon[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
					Demon[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
					Demon[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
					Demon[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
					Demon[i].respawnTimer = respawnTimerValue;    //复活计时器
					Demon[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
					Demon[i].attackCoolTime = 60;
					Demon[i].isEnemyImmunity = false;
					Demon[i].isStop = false;
					Demon[i].isStopTimer = 10;

					Dragon[i].Position.x = -100.0f;                //world position
					Dragon[i].Position.y = -100.0f;                //world position
					Dragon[i].moveVelocity.x = 1.5f;               //移动速度
					Dragon[i].moveVelocity.y = 1.5f;               //移动速度
					Dragon[i].width = 64;
					Dragon[i].height = 64;
					Dragon[i].attribute = DRAGON_FANTASTIC;        //属性
					Dragon[i].lifePoint = 120;                     //生命值
					Dragon[i].lifePointMax = 120;                  //生命值
					Dragon[i].attack = 10;                         //攻击力
					Dragon[i].defense = 5;                         //防御力
					Dragon[i].isEnemyAlive = false;                //生存flag 
					Dragon[i].isEnemyRealAlive = true;             //生存flag2
					Dragon[i].isMove = false;                      //移动flag
					Dragon[i].vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
					Dragon[i].vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
					Dragon[i].vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
					Dragon[i].vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
					Dragon[i].length = 0.0f;                       // 计算距离及正规化的时候用的参量
					Dragon[i].respawnTimer = respawnTimerValue;    //复活计时器
					Dragon[i].enemyAppearTimer = enemyAppearTimerValue;//出现计时器
					Dragon[i].attackCoolTime = 60;
					Dragon[i].isEnemyImmunity = false;
					Dragon[i].isStop = false;
					Dragon[i].isStopTimer = 10;
				}

				Boss.Position.x = -100.0f;                //world position
				Boss.Position.y = -100.0f;                //world position
				Boss.moveVelocity.x = 3.0f;               //移动速度
				Boss.moveVelocity.y = 3.0f;               //移动速度
				Boss.width = 128;                          //宽
				Boss.height = 128;                         //高
				Boss.attribute = BOSS_BOSS;           //属性
				Boss.lifePoint = 5000;                      //生命值
				Boss.lifePointMax = 5000;                   //生命值
				Boss.attack = 50;                          //攻击力
				Boss.defense = 30;                         //防御力
				Boss.isEnemyAlive = false;                //生存flag 
				Boss.isEnemyRealAlive = true;             //生存flag2
				Boss.isMove = false;                      //移动flag
				Boss.vector.x = 0.0f;                     // 计算距离及正规化的时候用的参量
				Boss.vector.y = 0.0f;                     // 计算距离及正规化的时候用的参量
				Boss.vectorNew.x = 0.0f;                  // 计算距离及正规化的时候用的参量
				Boss.vectorNew.y = 0.0f;                  // 计算距离及正规化的时候用的参量
				Boss.length = 0.0f;                       // 计算距离及正规化的时候用的参量
				Boss.respawnTimer = respawnTimerValue;    //复活计时器
				Boss.enemyAppearTimer = enemyAppearTimerValue;//出现计时器
				Boss.attackCoolTime = 0;//攻击计时器
				Boss.isEnemyImmunity = false;
				for (int i = 0; i < 32; i++) {
					bulletBoss[i].Position.x = -128;
					bulletBoss[i].Position.y = -128;
					bulletBoss[i].width = 32;
					bulletBoss[i].height = 32;
					bulletBoss[i].velocity = 6;
					bulletBoss[i].isShoot = false;
					bulletBoss[i].isExist = false;
				}



				player.Position.x = 640.0f;
				player.Position.y = 500.0f;
				player.moveVelocity.x = 5;
				player.moveVelocity.y = 5;
				player.moveVelocityMax.x = 5;
				player.moveVelocityMax.y = 5;
				player.width = 64.0f;
				player.height = 64.0f;
				Player_character = WARRIOR;
				Player_attribute = PHYSICS;
				player.job = Player_character;
				player.attribute = Player_attribute;
				player.lifePoint = 50.0f;
				player.lifePointMax = 50.0f;
				player.attack = 20.0f;
				player.defense = 15.0f;
				player.shootCoolTime = 0;
				player.velocityLength = 0;
				player.normalizeVelocity.x = 0;
				player.normalizeVelocity.y = 0;
				player.isTurnRight = false;
				player.level = 0;
				player.levelMax = 25;
				player.experience = 0;
				player.experienceMax = 0;
				player.levelUPExperience[player.levelMax + 1];
				for (int i = 0; i < player.levelMax + 1; i++) {
					player.levelUPExperience[i] = 100 * i;
				}
				player.isInvincible = false;
				player.invincibleTimer = 30;

				moneyNumber = 0;
				isStagePassed = false;
				ShopSelection = ITEM_1;
				isBought1 = false;
				isBought2 = false;
				isBought3 = false;
				isOnRiver = false;
				scene = START;
			}
#pragma endregion
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

#pragma region	シーンの切り替えの描画
		//=============================================
		// シーンの切り替え
		//=============================================
		//シーン
		switch (scene) {
			case Scene::START:

				//背景
				Novice::DrawSprite(
					0, 0, TEXTURE_START_BACKGROUND_TITLE,
					1, 1, 0.0f, WHITE
				);

				//選択肢
				Novice::DrawSprite(
					640, 240,
					TEXTURE_START_BACKGROUND_SELECTION[0],
					1, 1, 0.0f, WHITE
				);

				Novice::DrawSprite(
					640, 360,
					TEXTURE_START_BACKGROUND_SELECTION[2],
					1, 1, 0.0f, WHITE
				);

				Novice::DrawSprite(
					640, 480,
					TEXTURE_START_BACKGROUND_SELECTION[4],
					1, 1, 0.0f, WHITE
				);


				//起始页面 三个选项 含选择框 和 变色凸显
				switch (StartSelect) {
					case START_SELECT::GAMESTART_SELECT:
						//选择方框
						Novice::DrawSprite(
							620, 230,
							TEXTURE_START_BACKGROUND_SELECTION[6],
							1, 1, 0.0f, WHITE
						);

						//选项变色（后续作图时 画两种背景的图，这个用显眼色）
						Novice::DrawSprite(
							640, 240,
							TEXTURE_START_BACKGROUND_SELECTION[1],
							1, 1, 0.0f, WHITE
						);
						break;

					case START_SELECT::CHARACTER_SELECT:

						Novice::DrawSprite(
							620, 350,
							TEXTURE_START_BACKGROUND_SELECTION[6],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							640, 360,
							TEXTURE_START_BACKGROUND_SELECTION[3],
							1, 1, 0.0f, WHITE
						);
						break;

					case START_SELECT::OPTION_SELECT:

						Novice::DrawSprite(
							620, 470,
							TEXTURE_START_BACKGROUND_SELECTION[6],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							640, 480,
							TEXTURE_START_BACKGROUND_SELECTION[5],
							1, 1, 0.0f, WHITE
						);
						break;
				}
				break;

				//过场动画
			case Scene::LOADING:

				Novice::DrawSprite(
					0, 0,
					TEXTURE_START_BACKGROUND_LOADING,
					1, 1, 0.0f, WHITE
				);
				break;

				//角色介绍界面
			case Scene::CHARACTER:

				switch (CharacterIntroduction) {
					case CHARACTER_INTRODUCTION::WARRIOR_INTRODUCTION:
						Novice::DrawSprite(
							0, 0, TEXTURE__START_BACKGROUND_CHARACTER[0], 1, 1, 0.0f, WHITE
						);
						break;
					case CHARACTER_INTRODUCTION::MAGE_INTRODUCTION:
						Novice::DrawSprite(
							0, 0, TEXTURE__START_BACKGROUND_CHARACTER[1], 1, 1, 0.0f, WHITE
						);
						break;
					case CHARACTER_INTRODUCTION::PRIEST_INTRODUCTION:
						Novice::DrawSprite(
							0, 0, TEXTURE__START_BACKGROUND_CHARACTER[2], 1, 1, 0.0f, WHITE
						);
						break;
				}
				break;
				//选项界面（其实是操作方法）
			case Scene::OPTION:
				Novice::DrawSprite(
					0, 0, TEXTURE__START_BACKGROUND_OPTION, 1, 1, 0.0f, WHITE
				);
				break;
			case Scene::STAGE1:

				break;
		}
#pragma endregion
#pragma region ゲーム中の描画

		//=============================================
		// 游戏中
		//=============================================

		if (scene == STAGE1) {
#pragma region 背景

			Novice::DrawSprite(
				0, 0,
				TEXTURE_GAME_BACKGROUND_GAMING1,
				1, 1, 0.0f, WHITE
			);

#pragma endregion
#pragma region 横幅
			Novice::DrawSprite(
				(int)Banner_Stage1.x,
				(int)Banner_Stage1.y,
				TEXTURE_GAME_BANNER[0],
				1, 1, 0.0f, WHITE
			);
#pragma endregion
		}
		if (scene == STAGE2) {
#pragma region 背景
			Novice::DrawSprite(
				0, 0,
				TEXTURE_GAME_BACKGROUND_GAMING2,
				1, 1, 0.0f, WHITE
			);

#pragma endregion
#pragma region マップチップ
			for (int y = 0; y < 12; y++) {
				for (int x = 0; x < 20; x++) {
					if (map[y][x] == 1) {
						Novice::DrawSprite(
							x * size,
							y * size,
							TEXTURE_GAME_BACKGROUND_GAMING3,
							1, 1, 0.0f, WHITE
						);
					}
				}
			}
#pragma endregion
#pragma region 横幅
			Novice::DrawSprite(
				(int)Banner_Stage2.x,
				(int)Banner_Stage2.y,
				TEXTURE_GAME_BANNER[1],
				1, 1, 0.0f, WHITE
			);
#pragma endregion
		}
		if (scene == BOSS) {
#pragma region 背景
			if (gameTimer >= 0) {
				Novice::DrawSprite(
					0, 0,
					TEXTURE_GAME_BACKGROUND_BOSS[3],
					1, 1, 0.0f, WHITE
				);
			}
			if (gameTimer > 15) {
				Novice::DrawSprite(
					0, 0,
					TEXTURE_GAME_BACKGROUND_BOSS[2],
					1, 1, 0.0f, WHITE
				);
			}
			if (gameTimer > 30) {
				Novice::DrawSprite(
					0, 0,
					TEXTURE_GAME_BACKGROUND_BOSS[1],
					1, 1, 0.0f, WHITE
				);
			}
			if (gameTimer > 45 && gameTimer <= timerValue) {
				Novice::DrawSprite(
					0, 0,
					TEXTURE_GAME_BACKGROUND_BOSS[0],
					1, 1, 0.0f, WHITE
				);
			}




#pragma endregion
#pragma region 横幅
			Novice::DrawSprite(
				(int)Banner_Boss.x,
				(int)Banner_Boss.y,
				TEXTURE_GAME_BANNER[2],
				1, 1, 0.0f, WHITE
			);
#pragma endregion
		}
		if (scene == END1) {//be
			Novice::DrawSprite(
				0, 0, TEXTURE_BE,
				1, 1, 0.0f, WHITE
			);
		}
		if (scene == END2) {//he
			Novice::DrawSprite(
				0, 0, TEXTURE_HE,
				1, 1, 0.0f, WHITE
			);
		}
		if (scene == STAGE1 || scene == STAGE2 || scene == BOSS) {
#pragma region 金钱描画

			Novice::DrawSprite(5, 95, moneyTexture, 1, 1, 0.0f, WHITE);//金币图标
			//金币数字
			for (int i = 0; i < 4; i++) {

				Novice::DrawSprite(25 + 16 * i, 95, moneyNumberTexture[eachMoneyNumber[i]], 1, 1, 0.0f, WHITE);
			}
#pragma endregion
#pragma region 计时器描画
			for (int i = 0; i < 2; i++) {

				Novice::DrawSprite(608 + 32 * i + 10, 5, timerNumberTexture[eachTimerNumber[i]], 1, 1, 0.0f, WHITE);
			}
#pragma endregion
#pragma region 子弹描画
			//
#pragma region 战士子弹
			//战士
			if (player.job == WARRIOR) {
				for (int i = 0; i < 50; i++) {
					if (warriorBullet[i].isShoot && warriorBullet[i].isExist) {
						Novice::DrawSprite(
							(int)warriorBullet[i].Position.x,
							(int)warriorBullet[i].Position.y,
							TEXTURE_WARRIOR_BULLET,
							1, 1, 0.0f, WHITE
						);
					}
				}
			}
#pragma endregion
#pragma region 法师子弹
			//法师
			if (player.job == MAGE) {
				for (int i = 0; i < 50; i++) {
					if (mageBullet[i].isShoot && mageBullet[i].isExist) {
						Novice::DrawSprite(
							(int)mageBullet[i].Position.x,
							(int)mageBullet[i].Position.y,
							TEXTURE_MAGE_BULLET,
							1, 1, 0.0f, WHITE
						);
					}
				}
			}
#pragma endregion
#pragma region 牧师子弹
			//牧师
			if (player.job == PRIEST) {
				for (int i = 0; i < 50; i++) {
					if (priestBullet[i].isShoot && priestBullet[i].isExist) {
						Novice::DrawSprite(
							(int)priestBullet[i].Position.x,
							(int)priestBullet[i].Position.y,
							TEXTURE_PRIEST_BULLET,
							1, 1, 0.0f, WHITE
						);
					}
				}
			}
#pragma endregion
#pragma endregion
#pragma region player描画
			//player描画
			//血条
			Novice::DrawSprite(
				5, 5, TEXTURE_PLAYER_LIFEBAR,
				1, 1, 0.0f, WHITE
			);
			Novice::DrawBox(
				10, 10,
				(int)(player.lifePoint * 128 / player.lifePointMax),
				32,
				0.0f, RED, kFillModeSolid
			);
			//经验
			Novice::DrawSprite(
				5, 47, TEXTURE_PLAYER_LIFEBAR,
				1, 1, 0.0f, WHITE
			);
			Novice::DrawBox(
				10, 52,
				(int)(player.experience * 128 / player.experienceMax),
				32,
				0.0f, 0xFFFF00FF, kFillModeSolid
			);
			//人物
			switch (player.job) {
				case WARRIOR:
					if (player.isTurnRight == false) {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_WARRIOR[0],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x - 32,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_WARRIOR[0],
							1, 1, 0.0f, WHITE
						);
					}
					else {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_WARRIOR[1],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x + 64,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_WARRIOR[1],
							1, 1, 0.0f, WHITE
						);
					}
					break;
				case MAGE:
					if (player.isTurnRight == false) {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_MAGE[0],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x - 32,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_MAGE[0],
							1, 1, 0.0f, WHITE
						);
					}
					else {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_MAGE[1],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x + 64,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_MAGE[1],
							1, 1, 0.0f, WHITE
						);
					}
					break;
				case PRIEST:
					if (player.isTurnRight == false) {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_PRIEST[0],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x - 32,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_PRIEST[0],
							1, 1, 0.0f, WHITE
						);
					}
					else {
						Novice::DrawSprite(
							(int)player.Position.x,
							(int)player.Position.y,
							TEXTURE_PLAYER_PRIEST[1],
							1, 1, 0.0f, WHITE
						);
						Novice::DrawSprite(
							(int)player.Position.x + 64,
							(int)player.Position.y + 16,
							TEXTURE_WEAPON_PRIEST[1],
							1, 1, 0.0f, WHITE
						);
					}
					break;
			}
#pragma endregion
#pragma region 敌人描画
#pragma region goblin描画
			//goblin
			// 生成之后的各种表现
			for (int i = 0; i < 50; i++) {
				if (Goblin[i].isEnemyRealAlive) {
					if (Goblin[i].isEnemyAlive) {
						if (Goblin[i].isMove) {
							Novice::DrawSprite(
								(int)Goblin[i].Position.x,
								(int)Goblin[i].Position.y,
								TEXTURE_ENEMY_GOBLIN,
								1, 1, 0.0f, WHITE
							);
							Novice::DrawBox(
								(int)Goblin[i].Position.x,
								(int)Goblin[i].Position.y + (int)Goblin[i].height,
								(int)(Goblin[i].lifePoint * 64 / Goblin[i].lifePointMax),
								10,
								0.0f, RED, kFillModeSolid
							);
						}
						//生成预警
						else {
							if (Goblin[i].enemyAppearTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Goblin[i].Position.x,
									(int)Goblin[i].Position.y,
									TEXTURE_ENEMY_BORN,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
					//死亡特效
					else {
						if (Goblin[i].respawnTimer > (respawnTimerValue - 30)) {
							if (Goblin[i].respawnTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Goblin[i].Position.x,
									(int)Goblin[i].Position.y,
									TEXTURE_ENEMY_DEATH,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
				}
			}
#pragma endregion
#pragma region Ghost描画
			//Ghost
			// 生成之后的各种表现
			for (int i = 0; i < 50; i++) {
				if (Ghost[i].isEnemyRealAlive) {
					if (Ghost[i].isEnemyAlive) {
						if (Ghost[i].isMove) {
							Novice::DrawSprite(
								(int)Ghost[i].Position.x,
								(int)Ghost[i].Position.y,
								TEXTURE_ENEMY_GHOST,
								1, 1, 0.0f, WHITE
							);
							Novice::DrawBox(
								(int)Ghost[i].Position.x,
								(int)Ghost[i].Position.y + (int)Ghost[i].height,
								(int)(Ghost[i].lifePoint * 64 / Ghost[i].lifePointMax),
								10,
								0.0f, RED, kFillModeSolid
							);
							if (Ghost[i].isEnemyImmunity) {
								Novice::DrawSprite(
									(int)Ghost[i].Position.x,
									(int)Ghost[i].Position.y - 16,
									TEXTURE_ENEMY_IMMUNITY,
									1, 1, 0.0f, WHITE
								);

							}
						}
						else {
							if (Ghost[i].enemyAppearTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Ghost[i].Position.x,
									(int)Ghost[i].Position.y,
									TEXTURE_ENEMY_BORN,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
					else {
						if (Ghost[i].respawnTimer > (respawnTimerValue - 30)) {
							if (Ghost[i].respawnTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Ghost[i].Position.x,
									(int)Ghost[i].Position.y,
									TEXTURE_ENEMY_DEATH,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
				}
			}
#pragma endregion
#pragma region Demon描画
			//Demon
			// 生成之后的各种表现
			for (int i = 0; i < 50; i++) {
				if (Demon[i].isEnemyRealAlive) {
					if (Demon[i].isEnemyAlive) {
						if (Demon[i].isMove) {
							Novice::DrawSprite(
								(int)Demon[i].Position.x,
								(int)Demon[i].Position.y,
								TEXTURE_ENEMY_DEMON,
								1, 1, 0.0f, WHITE
							);
							Novice::DrawBox(
								(int)Demon[i].Position.x,
								(int)Demon[i].Position.y + (int)Demon[i].height,
								(int)(Demon[i].lifePoint * 64 / Demon[i].lifePointMax),
								10,
								0.0f, RED, kFillModeSolid
							);
						}
						else {
							if (Demon[i].enemyAppearTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Demon[i].Position.x,
									(int)Demon[i].Position.y,
									TEXTURE_ENEMY_BORN,
									1, 1, 0.0f, WHITE
								);
							}
						}

					}
					else {
						if (Demon[i].respawnTimer > (respawnTimerValue - 30)) {
							if (Demon[i].respawnTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Demon[i].Position.x,
									(int)Demon[i].Position.y,
									TEXTURE_ENEMY_DEATH,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
				}
			}
#pragma endregion
#pragma region Dragon描画
			//Dragon
			// 生成之后的各种表现
			for (int i = 0; i < 50; i++) {
				if (Dragon[i].isEnemyRealAlive) {
					if (Dragon[i].isEnemyAlive) {
						if (Dragon[i].isMove) {
							Novice::DrawSprite(
								(int)Dragon[i].Position.x,
								(int)Dragon[i].Position.y,
								TEXTURE_ENEMY_DRAGON,
								1, 1, 0.0f, WHITE
							);
							Novice::DrawBox(
								(int)Dragon[i].Position.x,
								(int)Dragon[i].Position.y + (int)Dragon[i].height,
								(int)(Dragon[i].lifePoint * 64 / Dragon[i].lifePointMax),
								10,
								0.0f, RED, kFillModeSolid
							);
							if (Dragon[i].isEnemyImmunity) {
								Novice::DrawSprite(
									(int)Dragon[i].Position.x,
									(int)Dragon[i].Position.y - 16,
									TEXTURE_ENEMY_IMMUNITY,
									1, 1, 0.0f, WHITE
								);
							}
						}
						else {
							if (Dragon[i].enemyAppearTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Dragon[i].Position.x,
									(int)Dragon[i].Position.y,
									TEXTURE_ENEMY_BORN,
									1, 1, 0.0f, WHITE
								);
							}
						}

					}
					else {
						if (Dragon[i].respawnTimer > (respawnTimerValue - 30)) {
							if (Dragon[i].respawnTimer % 2 == 0) {
								Novice::DrawSprite(
									(int)Dragon[i].Position.x,
									(int)Dragon[i].Position.y,
									TEXTURE_ENEMY_DEATH,
									1, 1, 0.0f, WHITE
								);
							}
						}
					}
				}
			}
#pragma endregion
#pragma endregion
#pragma region boss描画
			//Boss
			// 生成之后的各种表现
			if (Boss.isEnemyRealAlive) {
				if (Boss.isEnemyAlive) {
					if (Boss.isMove) {
						for (int i = 0; i < 32; i++) {
							if (bulletBoss[i].isExist) {
								Novice::DrawSprite(
									(int)bulletBoss[i].Position.x,
									(int)bulletBoss[i].Position.y,
									TEXTURE_BOSS_BULLET,
									1, 1, 0.0f, WHITE
								);
							}
						}
						Novice::DrawSprite(
							(int)Boss.Position.x,
							(int)Boss.Position.y,
							TEXTURE_ENEMY_BOSS,
							1, 1, 0.0f, WHITE
						);
						Novice::DrawBox(
							(int)Boss.Position.x,
							(int)Boss.Position.y + (int)Boss.height,
							(int)(Boss.lifePoint * 128 / Boss.lifePointMax),
							10,
							0.0f, RED, kFillModeSolid
						);
						if (Boss.isEnemyImmunity) {
							Novice::DrawSprite(
								(int)Boss.Position.x,
								(int)Boss.Position.y - 16,
								TEXTURE_ENEMY_IMMUNITY,
								1, 1, 0.0f, WHITE
							);
						}
					}
					else {
						if (Boss.enemyAppearTimer % 2 == 0) {
							Novice::DrawSprite(
								(int)Boss.Position.x,
								(int)Boss.Position.y,
								TEXTURE_ENEMY_BORN,
								1, 1, 0.0f, WHITE
							);
						}
					}

				}
				else {
					if (Boss.respawnTimer > (respawnTimerValue - 30)) {
						if (Boss.respawnTimer % 2 == 0) {
							Novice::DrawSprite(
								(int)Boss.Position.x,
								(int)Boss.Position.y,
								TEXTURE_ENEMY_DEATH,
								1, 1, 0.0f, WHITE
							);
						}
					}
				}
			}
#pragma endregion

			if (isPaused) {
				Novice::DrawBox(
					0, 100,
					1280, 200,
					0.0f, RED, kFillModeSolid
				);
				Novice::ScreenPrintf(500, 100, "speed=%f", player.moveVelocityMax.x);
				Novice::ScreenPrintf(500, 125, "speed=%f", player.moveVelocityMax.y);
				Novice::ScreenPrintf(500, 150, "life=%f", player.lifePointMax);
				Novice::ScreenPrintf(500, 175, "atk=%f", player.attack);
				Novice::ScreenPrintf(500, 200, "def=%f", player.defense);
			}

		}

#pragma endregion
#pragma region StagePassedのReward描画
		if (scene == STAGE_PASSED_REWARD) {
			//背景
			Novice::DrawSprite(
				0, 0,
				TEXTURE_BUFF_REWARD,
				1, 1, 0.0f, WHITE
			);
			switch (RewardSelection) {
				case REWARD_1:
					/*Novice::DrawBox(
						110, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						110, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
				case REWARD_2:
					/*Novice::DrawBox(
						500, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						500, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
				case REWARD_3:
					/*Novice::DrawBox(
						890, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						890, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
			}
			Novice::DrawSprite(
				120, 120,
				TEXTURE_BUFF_ON[1],
				1, 1, 0.0f, WHITE
			);
			Novice::DrawSprite(
				510, 120,
				TEXTURE_BUFF_ON[2],
				1, 1, 0.0f, WHITE
			);
			Novice::DrawSprite(
				900, 120,
				TEXTURE_BUFF_ON[3],
				1, 1, 0.0f, WHITE
			);


		}
#pragma endregion
#pragma region SHOP描画
		if (scene == SHOP) {
			//背景
			Novice::DrawSprite(
				0, 0,
				TEXTURE_SHOP_BACKGROUND,
				1, 1, 0.0f, WHITE
			);
			Novice::DrawSprite(5, 95, moneyTexture, 1, 1, 0.0f, WHITE);//金币图标
			//金币数字
			for (int i = 0; i < 4; i++) {

				Novice::DrawSprite(25 + 16 * i, 95, moneyNumberTexture[eachMoneyNumber[i]], 1, 1, 0.0f, WHITE);
			}
			switch (ShopSelection) {
				case ITEM_1:
					/*Novice::DrawBox(
						110, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						110, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
				case ITEM_2:
					/*Novice::DrawBox(
						500, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						500, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
				case ITEM_3:
					/*Novice::DrawBox(
						890, 110,
						280, 500,
						0.0f, RED, kFillModeSolid
					);*/
					Novice::DrawSprite(
						890, 110,
						TEXTURE_SELECTION,
						1, 1, 0.0f, WHITE
					);
					break;
			}
			if (!isBought1) {
				Novice::DrawSprite(
					120, 120,
					TEXTURE_ITEM_ON[1],
					1, 1, 0.0f, WHITE
				);
			}
			if (!isBought2) {
				Novice::DrawSprite(
					510, 120,
					TEXTURE_ITEM_ON[2],
					1, 1, 0.0f, WHITE
				);
			}
			if (!isBought3) {
				Novice::DrawSprite(
					900, 120,
					TEXTURE_ITEM_ON[3],
					1, 1, 0.0f, WHITE
				);
			}
		}
#pragma endregion
#pragma region DEATH描画
		if (scene == DEATH) {
			Novice::DrawSprite(
				0, 0, TEXTURE_DEATH,
				1, 1, 0.0f, WHITE
			);
		}
#pragma endregion
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
