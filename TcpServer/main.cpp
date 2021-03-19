#include <iostream>
#include <list>
#include <sstream>
#include <conio.h>
#include "EventQueue.h"
#include "HttpServer.h"
#include "Parameters.h"
#include "VolumeControl.h"
#include "Callback.h"
#include "IrAnalyxerUtils.h"

HttpServer* _http;

int kbHit(void)
{
	if (GetKeyState('X') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
	{
		return 1;
	}

	return 0;
}

void threadFunc()
{
	message_t msg;
	bool running = true;
	
while (running) {
	EventQueue::instance()->wait(msg);
	switch (msg.event) {
	case EVENT_HTTP_REQUEST_POST_SET_VOLUME:
	{
		int level = static_cast<int>(msg.data);
		if (level >= 0) {
			//uint32_t data = VolumeControl::Update | (level << 2);
			//EventQueue::instance()->post(EVENT_VOLUME_COMMAND, data);
			Parameters::instance()->current_level = level & 0xFF;
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_POST);
		}
		else {
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
		}
		break;
	}
	case EVENT_HTTP_REQUEST_POST_SET_INPUT:
	{
		Spdif::InputT input = Parameters::instance()->current_input;

		if (SpdifHelper::isValidInput(static_cast<int>(msg.data), &input))
		{
			Parameters::instance()->auto_find = false;
			if (input != Parameters::instance()->current_input) {
				//EventQueue::instance()->post(EVENT_CHANGE_INPUT_REQ, input);
				Parameters::instance()->current_input = input;
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
			}
			else {
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
			}
		}
		else {
			Parameters::instance()->auto_find = true;
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
		}
		break;
	}
	case EVENT_HTTP_REQUEST_POST_SET_SWITCH_ORDER:
	{
		std::stringstream ss;

		int listSize = msg.data & 0x3;
		for (int i = 0; i < listSize; i++) {
			msg.data = msg.data >> 2;
			ss << (msg.data & 0x3);
			if (i < (listSize - 1)) {
				ss << ",";
			}
		}

		std::vector<Spdif::InputT> switchPriorityOrder = SpdifHelper::parseSwitchPriorityOrder(ss.str());
		Parameters::instance()->setSwitchOrderList(switchPriorityOrder);

		EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_SWITCH_UPDATE);
		break;
	}
	case EVENT_HTTP_SEND_RESPONSE:
	{
		char buffer[64] = { '\0' };
		HttpResponseTypeT responseType = static_cast<HttpResponseTypeT>(msg.data);

		if (responseType == HTTP_RESPONSE_POST) {
			sprintf_s(buffer, "%d", Parameters::instance()->current_level);
		}
		else if (responseType == HTTP_RESPONSE_INFO_UPDATE) {
			char inputTitle[10] = { 0 };
			char sampleRateTitle[10] = { 0 };
			char pcmTitle[10] = { 0 };
			uint32_t data = Parameters::instance()->getSpdifStatus(Parameters::instance()->current_input);

			SpdifHelper::getInputInfo(data, inputTitle, sampleRateTitle, pcmTitle);
			sprintf_s(buffer, "%d;%s;%d;%s %s",
				Parameters::instance()->current_level,
				inputTitle,
				Parameters::instance()->auto_find ? 1 : 0,
				sampleRateTitle,
				pcmTitle);
		}
		else if (responseType == HTTP_RESPONSE_SWITCH_UPDATE) {
			std::string list = Parameters::instance()->getSwitchOrderListStr();
			sprintf_s(buffer, "%s", list.c_str());
		}
		_http->sendResponse(responseType, buffer);
		break;
	}
	case EVENT_SYSTEM_SHUTDOWN:
		puts("system shutdown");
		running = false;
		break;
	default:
		break;
	}
}
}

class Base {
public:
	Base() {};
	void set(int i)
	{
		printf("base: %d\n", i);
		_value = ++i;
		this->setImp();
	}
protected:
	virtual void setImp() = 0;
	int _value;
};

class Ment : public Base {
public:
	Ment() {};
private:
	void setImp()
	{
		printf("ment: %d\n", _value);
	}
};

void showInfoMenu()
{
	puts("Info");
}

void showNetworkMenu()
{
	puts("Network settings");
}

void showUpdateMenu()
{
	puts("Update device");
}

void showRebootMenu()
{
	puts("Reboot and shutdown");
}

class IMenu;

class MenuItem {
public:
	MenuItem(std::string p_title, int p_index)
	{
		_title = p_title;
		_index = p_index;
		_menu = NULL;
		_callback = NULL;
	}

	MenuItem(std::string p_title, int p_index, Callback<void()> p_callback)
	{
		_title = p_title;
		_index = p_index;
		_callback = p_callback;
		_menu = NULL;
	}

	MenuItem(std::string p_title, int p_index, IMenu* p_menu)
	{
		_title = p_title;
		_index = p_index;
		_callback = NULL;
		_menu = p_menu;
	}

	~MenuItem()
	{
		std::cout << _title << " destructed" << std::endl;
	}

	std::string getTitle()
	{
		return _title;
	}

	int getIndex()
	{
		return _index;
	}

	IMenu* getMenu()
	{
		return _menu;
	}

	void performCallback()
	{
		_callback();
	}

private:
	std::string _title;
	int _index;
	Callback<void()> _callback;
	IMenu* _menu;
};

class IMenu
{
public:
	~IMenu()
	{
		std::vector<MenuItem*>::const_iterator iter = _menuItems.begin();
		std::vector<MenuItem*>::const_iterator iterEnd = _menuItems.end();
		while (iter != iterEnd) {
			delete (*iter);
			++iter;
		}
		_menuItems.clear();
	}

	void draw()
	{
		system("cls");
		std::vector<MenuItem*>::const_iterator iter = _menuItems.begin();
		std::vector<MenuItem*>::const_iterator iterEnd = _menuItems.end();
		while (iter != iterEnd) {
			if ((*iter)->getIndex() == _selectorIndex) {
				printf("* %s\n", (*iter)->getTitle().c_str());
			}
			else {
				printf("  %s\n", (*iter)->getTitle().c_str());
			}
			++iter;
		}
	}

	void next()
	{
		if (_menuItems.size() == 0) {
			return;
		}

		if (++_selectorIndex == _menuItems.size()) {
			_selectorIndex = _menuItems.size() - 1;
		}
	}

	void previous()
	{
		if (_menuItems.size() == 0) {
			return;
		}

		if (--_selectorIndex < 0) {
			_selectorIndex = 0;
		}
	}

	IMenu* select()
	{
		IMenu* menu = _menuItems[_selectorIndex]->getMenu();
		if (menu != NULL) {
			menu->draw();
		}
		else {
			menu = this;
			_menuItems[_selectorIndex]->performCallback();
		}
		return menu;
	}

protected:
	IMenu() : _selectorIndex(0)
	{
	};

	void addItem(std::string p_title)
	{
		_menuItems.push_back(new MenuItem(p_title, _menuItems.size()));
	}

	void addItem(std::string p_title, Callback<void()> p_callback)
	{
		_menuItems.push_back(new MenuItem(p_title, _menuItems.size(), p_callback));
	}

	void addItem(std::string p_title, IMenu* p_menu)
	{
		_menuItems.push_back(new MenuItem(p_title, _menuItems.size(), p_menu));
	}

	MenuItem* getItem(int p_index)
	{
		std::vector<MenuItem*>::const_iterator iter = _menuItems.begin();
		std::vector<MenuItem*>::const_iterator iterEnd = _menuItems.end();
		while (iter != iterEnd) {
			if ((*iter)->getIndex() == p_index) {
				return (*iter);
			}
			++iter;
		}

		return NULL;
	}

	int getIndex()
	{
		return _selectorIndex;
	}

private:
	int _selectorIndex;
	std::vector<MenuItem*> _menuItems;
};

class InfoMenu : public IMenu {
public:
	InfoMenu(IMenu* p_parent)
	{
		addItem("SW version", callback(this, &InfoMenu::showSwVersion));
		addItem("HW version", callback(this, &InfoMenu::showHwVersion));
		addItem("Exit", p_parent);
	}

	virtual ~InfoMenu()
	{
		puts("InfoMenu destructor");
	}
	
	void select()
	{
		puts("Info select");
	}

private:
	void showSwVersion()
	{
		puts("SW: 3.0.2");
	}

	void showHwVersion()
	{
		puts("HW: 1.1");
	}
};

class MainMenu : public IMenu {
public:
	MainMenu()
	{
		_info = new InfoMenu(this);
		addItem("Info", _info);
		//addItem("Network", nullptr);
		//addItem("Update", nullptr);
		//addItem("Reboot", nullptr);
		//addItem("Exit", nullptr);
	}

	~MainMenu()
	{
		delete _info;
	}

private:
	InfoMenu* _info;
};

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void traverseString(std::string& p_str)
{
	for (auto& ch : p_str) {
		//std::cout << ch << " ";
		printf("%02X", ch);
	}
	puts("");
}

void analyzeIr()
{
	static std::map<IrCommandT, std::vector<uint32_t>> recordedIrCodeMap;

	IrCommandT _irCommand = IR_VOLUME_UP;
	std::vector<uint32_t> upCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B084, 0xB0B08434, 0xB0843434, 0x84343484, 0x343484B0, 0x3484B030};
	std::vector<uint32_t> downCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B0B0, 0xB0B0B034, 0xB0B034B4, 0xB034B430, 0x34B430B0, 0xB430B030};
	std::vector<uint32_t> muteCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B084, 0xB0B084B0, 0xB084B034, 0x84B03484, 0xB03484B0, 0x3484B030};
	/*std::vector<int> bytes = IrAnalyzerUtils::analyze(buff, BUFF_SIZE);
	commands.push_back(bytes);
	IrAnalyzerUtils::logBytes(bytes);

	IrAnalyzerUtils::trimStart(commands);
	IrAnalyzerUtils::logMatrix(commands);

	std::vector<uint32_t> codes;
	IrAnalyzerUtils::getCodes(commands.front(), codes);*/
	
	recordedIrCodeMap[IR_VOLUME_UP] = upCodes;
	recordedIrCodeMap[IR_VOLUME_DOWN] = downCodes;
	recordedIrCodeMap[IR_VOLUME_MUTE] = muteCodes;
	IrAnalyzerUtils::logIrCodeMap(recordedIrCodeMap);

	std::map<IrCommandT, std::vector<uint32_t>> uniqueIrCodesMap;
	IrAnalyzerUtils::getUniqueIrCodesMap(uniqueIrCodesMap, recordedIrCodeMap);
	IrAnalyzerUtils::logIrCodeMap(uniqueIrCodesMap);

	uint32_t irCode = IrAnalyzerUtils::getScoredCode(uniqueIrCodesMap[_irCommand]);
	if (irCode != 0x0) {
		//std::vector<uint32_t> currentCodes = Parameters::instance()->getIrRemoteCodes();
		//currentCodes[_irCommand] = irCode;
		printf("Code: %08X\n", irCode);
		//Parameters::instance()->setIrRemoteCodesList(currentCodes);
	}
	else {
		perror("IR learning failed");
	}

}

int __cdecl main(void)
{
	analyzeIr();

	//std::vector<uint32_t> irCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B084, 0xB0B08434, 0xB0843484, 0x84348484, 0x348484B0, 0x8484B004};
	std::vector<uint32_t> irCodes = { 0x0030B0B4, 0x30B0B430, 0xB0B430B4, 0xB430B484, 0x30B48484, 0xB48484B4, 0x8484B484, 0x84B48430, 0xB4843030, 0x84303000 };

	uint32_t irCode = IrAnalyzerUtils::getScoredCode(irCodes);
	printf("The winner is: 0x%08X\n", irCode);

	/*
	std::map<int, uint32_t> orderedCodes;
	std::map<uint32_t, int>::const_iterator mapIter = irCodeCorrelationMap.cbegin();
	while (mapIter != irCodeCorrelationMap.cend()) {
		orderedCodes[mapIter->second] = mapIter->first;
		printf("0x%08X: %d\n", mapIter->first, mapIter->second);
		++mapIter;
	}

	if (!orderedCodes.empty()) {
		std::map<int, uint32_t>::const_reverse_iterator mapRevIter = orderedCodes.crbegin();
		while (mapRevIter != orderedCodes.crend()) {
			printf("The winner is: 0x%08X\n", mapRevIter->second);
			break;
		}
	}	*/
	getchar();

	/*std::string ssid = "Kuhaverkko\r\n";
	traverseString(ssid);

	std::string::size_type pos = 0;
	while ((pos = ssid.find("\r\n", pos)) != std::string::npos)
	{
		ssid.erase(pos, 2);
	}
	std::cout << ssid << std::endl;


	std::string s = "wlan0     ESSID:\"Kuhaverkko\"";
	//std::string s = "";
	std::vector<std::string> list = split(s, '"');
	if (list.size() == 2) {
		std::cout << list[1] << std::endl;
	}
	else {
		puts("WiFi not connected");
	}
	getchar();
	return 0;*/

	MainMenu mainMenu;

	IMenu* activeMenu = &mainMenu;
	activeMenu->draw();

	while (1)
	{
		//system("cls");
		int k = _getch();

		if (k == 0 || k == 0xE0) {
			k = _getch();
		}

		if (k == 27) {
			break;
		}
		else if (k == 13) {
			activeMenu = activeMenu->select();
		}
		else if (k == 72) {
			activeMenu->previous();
			activeMenu->draw();
		}
		else if (k == 80) {
			activeMenu->next();
			activeMenu->draw();
		}
	}
	return 0;
	//Ment m;
	//m.set(1);

	getchar();
	return 0;

	puts("GEVOL 3.0.0");
	puts("Print any key to exit...");

	bool b = Parameters::instance()->auto_find;
	VolumeControl volume(Parameters::instance()->getStartLevel());

	_http = new HttpServer();

	Spdif::StatusS msg;
	msg.input = Spdif::Coax1;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_44100;	
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	msg.input = Spdif::Coax2;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_192000;
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	msg.input = Spdif::Opt1;
	msg.pcm_info = Spdif::PCM_DD_AC3;
	msg.sample_rate = Spdif::SR_48000;
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	Parameters::instance()->current_level = 26;
	Parameters::instance()->current_input = Spdif::Coax1;
	Parameters::instance()->auto_find = true;

	std::thread t(threadFunc);

	printf("TCP server started!\n\n");
	while (!kbHit()) {
		Sleep(10);	
	}
	EventQueue::instance()->post(EVENT_SYSTEM_SHUTDOWN);
	t.join();

	delete _http;
	return 0;
}
