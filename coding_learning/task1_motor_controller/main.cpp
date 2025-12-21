// Motor controller demo that offers an interactive menu to manage speed and direction.
#include <clocale>
#include <iostream>
#include <limits>
#include <string>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {
constexpr int kMinSpeed = 0;
constexpr int kMaxSpeed = 100;
constexpr int kMenuMin = 0;
constexpr int kMenuMax = 6;
constexpr int kCruiseSpeed = 60;

#ifdef _WIN32
bool gUseAnsiColors = false;
#else
bool gUseAnsiColors = true;
#endif

constexpr const char* kAnsiRed = "\x1b[31m";
constexpr const char* kAnsiGreen = "\x1b[32m";
constexpr const char* kAnsiReset = "\x1b[0m";
}

class MotorController {
public:
	enum class Direction { Clockwise = 1, CounterClockwise = -1 };

	bool powerOn()
	{
		if (poweredOn) {
			std::cout << "电机已经开启。\n";
			return false;
		}
		poweredOn = true;
		std::cout << "电机已启动。\n";
		return true;
	}

	void powerOff()
	{
		if (!poweredOn) {
			std::cout << "电机已经关闭。\n";
			return;
		}
		poweredOn = false;
		speed = 0;
		std::cout << "电机已停止，速度重置为 0。\n";
	}

	bool isPowered() const { return poweredOn; }

	bool setSpeed(int newSpeed)
	{
		if (!poweredOn) {
			std::cout << "请先开启电机再调整速度。\n";
			return false;
		}
		const int maxAllowed = isSpeedLimited ? speedLimit : kMaxSpeed;
		if (newSpeed < kMinSpeed || newSpeed > maxAllowed) {
			std::cout << "速度必须在 " << kMinSpeed << " 到 " << maxAllowed << " 之间。\n";
			return false;
		}
		speed = newSpeed;
		std::cout << "速度已设置为 " << speed << "。\n";
		return true;
	}

	int getSpeed() const { return speed; }

	bool setDirection(Direction dir)
	{
		if (!poweredOn) {
			std::cout << "请先开启电机再调整方向。\n";
			return false;
		}
		direction = dir;
		std::cout << "方向已调整为 " << directionToString() << "。\n";
		return true;
	}

	Direction getDirection() const { return direction; }

	std::string directionToString() const
	{
		return direction == Direction::Clockwise ? "顺时针" : "逆时针";
	}

	bool toggleDirection()
	{
		const auto next = (direction == Direction::Clockwise) ? Direction::CounterClockwise : Direction::Clockwise;
		return setDirection(next);
	}

	bool setSpeedLimit(int limit)
	{
		if (limit < kMinSpeed || limit > kMaxSpeed) {
			std::cout << "最大速度限制必须在 " << kMinSpeed << " 到 " << kMaxSpeed << " 之间。\n";
			return false;
		}
		isSpeedLimited = true;
		speedLimit = limit;
		std::cout << "已设置最大速度限制为 " << speedLimit << "。\n";
		if (poweredOn && speed > speedLimit) {
			speed = speedLimit;
			std::cout << "当前速度已被限制到 " << speed << "。\n";
		}
		return true;
	}

	void clearSpeedLimit()
	{
		if (!isSpeedLimited) {
			std::cout << "当前没有启用最大速度限制。\n";
			return;
		}
		isSpeedLimited = false;
		speedLimit = kMaxSpeed;
		std::cout << "已解除最大速度限制。\n";
	}

	bool isMaxSpeedLimited() const { return isSpeedLimited; }
	int getSpeedLimit() const { return speedLimit; }

private:
	bool poweredOn = false;
	int speed = 0;
	Direction direction = Direction::Clockwise;
	bool isSpeedLimited = false;
	int speedLimit = kMaxSpeed;
};

std::string colorize(const std::string& text, const char* ansiColor)
{
	if (!gUseAnsiColors) {
		return text;
	}
	return std::string(ansiColor) + text + kAnsiReset;
}

std::string powerStateText(bool poweredOn)
{
	return poweredOn ? colorize("开启", kAnsiGreen) : colorize("关闭", kAnsiRed);
}

std::string makeSpeedBar(int speed, std::size_t width = 20)
{
	if (speed < kMinSpeed) {
		speed = kMinSpeed;
	}
	if (speed > kMaxSpeed) {
		speed = kMaxSpeed;
	}
	const auto filled = static_cast<std::size_t>(speed * static_cast<int>(width) / kMaxSpeed);
	std::string bar;
	bar.reserve(width);
	bar.append(filled, '#');
	bar.append(width - filled, '-');
	return bar;
}

void showMenu(const MotorController& motor)
{
	std::cout << "\n================ 电机控制台 ================\n";
	std::cout << "当前电机状态 => 电源:" << powerStateText(motor.isPowered())
			<< " | 方向:" << motor.directionToString()
			<< " | 限速:" << (motor.isMaxSpeedLimited() ? std::to_string(motor.getSpeedLimit()) : std::string("不限"))
			<< "\n";
	std::cout << "速度进度条   => [" << makeSpeedBar(motor.getSpeed()) << "] " << motor.getSpeed() << "%\n\n";

	std::cout << "0. 关闭电机（退出程序）\n";
	std::cout << "1. 开启电机\n";
	std::cout << "2. 设置电机速度 (0-100)\n";
	std::cout << "3. 改变旋转方向（直接切换）\n";
	std::cout << "4. 一键巡航（速度 60，顺时针）\n";
	std::cout << "5. 设置最大速度限制\n";
	std::cout << "6. 解除最大速度限制\n";
}

int readInt(const std::string& prompt, int minValue, int maxValue)
{
	while (true) {
		std::cout << prompt;
		int value;
		if (std::cin >> value) {
			if (value >= minValue && value <= maxValue) {
				return value;
			}
			std::cout << "数值必须在 " << minValue << " 与 " << maxValue << " 之间。\n";
		} else {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "请输入有效的数字。\n";
		}
	}
}

void configureConsoleEncoding()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	{
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (handle != INVALID_HANDLE_VALUE) {
			DWORD mode = 0;
			if (GetConsoleMode(handle, &mode)) {
				mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				if (SetConsoleMode(handle, mode)) {
					gUseAnsiColors = true;
				}
			}
		}
	}
#endif
	std::setlocale(LC_ALL, ".UTF-8");
}

int main()
{
	configureConsoleEncoding();
	MotorController motor;
	bool running = true;

	while (running) {
		showMenu(motor);
		int choice = readInt("请选择操作：", kMenuMin, kMenuMax);

		switch (choice) {
		case 0:
			motor.powerOff();
			std::cout << "程序结束，再见！\n";
			running = false;
			break;
		case 1:
			motor.powerOn();
			break;
		case 2: {
			int newSpeed = readInt("请输入目标速度：", kMinSpeed, kMaxSpeed);
			motor.setSpeed(newSpeed);
			break;
		}
		case 3: {
			motor.toggleDirection();
			break;
		}
		case 4:
			if (!motor.isPowered()) {
				motor.powerOn();
			}
			motor.setDirection(MotorController::Direction::Clockwise);
			if (motor.setSpeed(kCruiseSpeed)) {
				std::cout << "一键巡航已启用。\n";
			} else if (motor.isMaxSpeedLimited()) {
				motor.setSpeed(motor.getSpeedLimit());
				std::cout << "因限速，一键巡航速度调整为 " << motor.getSpeedLimit() << "。\n";
			}
			break;
		case 5: {
			int limit = readInt("请输入最大速度限制 (0-100)：", kMinSpeed, kMaxSpeed);
			motor.setSpeedLimit(limit);
			break;
		}
		case 6:
			motor.clearSpeedLimit();
			break;
		default:
			std::cout << "无效的菜单选项。\n";
			break;
		}
	}

	return 0;
}
