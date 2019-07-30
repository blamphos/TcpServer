#include "mbed.h"
#include "SystemControl.h"
#include "SpdifSwitch.h"
#include "VolumeControl.h"
#include "HttpServer.h"

SystemControl::SystemControl()
{
	_parameters = Parameters::instance();

	_volume_control = new VolumeControl(MPC4161_CS_PIN);
	_spdif_switch = new SpdifSwitch(INP1_EN_PIN, INP2_EN_PIN, INP3_EN_PIN);

	_http = new HttpServer();
	//_main_sw = new PushButtonSwitch(POWER_SW_PIN, ButtonBase::BUTTON_ID_MAIN_SW);
	//_input_sw = new PushButtonSwitch(INPUT_SW_PIN, ButtonBase::BUTTON_ID_INPUT_SW);
	//_rotary_sw = new RotaryEncoderSwitch(CLK_PIN, DT_PIN, ButtonBase::BUTTON_ID_ROTARY_SW);

	//_ir_receiver = new IrReceiver(IR_RX_PIN, TIMER_SYSTEM_TICK_US);
	//_spdif_decoder = new SpdifDecoder();

	//_display = Display::instance();

	//_system_ticker.attach_us(callback(this, &SystemControl::ticker_isr), TIMER_SYSTEM_TICK_US);
	EventQueue::instance()->post(EVENT_HTTP_SERVER_INIT);
}

SystemControl::~SystemControl()
{
    delete _http;
}

void SystemControl::handleMessage()
{
	message_t msg;
	if (EventQueue::instance()->getNext(msg)) {
		switch (msg.event) {
		case EVENT_VOLUME_COMMAND:
			_volume_control->handleCommand(msg.data);
			_spdif_switch->updateMuteState();
			//_display->handleMessage(msg);
			break;
		case EVENT_BUTTON_STATUS:
			//_display->handleMessage(msg);
			break;
		case EVENT_GET_SPDIF_STATUS:
			//_spdif_decoder->getStatus();
			break;
		case EVENT_UPDATE_SPDIF_STATUS:
			_spdif_switch->updateSpdifStatus(msg.data);
			//_display->handleMessage(msg);
			break;
		case EVENT_CHANGE_INPUT_REQ:
			_spdif_switch->switchInput(msg.data);
			break;
		case EVENT_INPUT_CHANGED:
			_volume_control->mute();
			_spdif_switch->mute();
			//_display->handleMessage(msg);
			break;
		case EVENT_CHANGE_SCREEN_STATE:
			_spdif_switch->unmute();
			_volume_control->unmute();
			//_display->handleMessage(msg);
			break;
		case EVENT_HTTP_SERVER_READY:
		    _http->handleMessage(msg);
			//_system_ticker.attach_us(callback(this, &SystemControl::ticker_isr), TIMER_SYSTEM_TICK_US);
			break;
		default:
		    _http->handleMessage(msg);
			break;
		}
	}

    if (EventQueue::instance()->isEmpty()) {
        wait_ms(1);
    }
}

void SystemControl::ticker_isr()
{
#ifndef SIMULATED
	static uint32_t ticks = 0;

	_ir_receiver->isr();

	++ticks;
	if(!(ticks & TIMER_MASK_1_6_MS)) {
		_input_sw->isr();
		_main_sw->isr();
		_rotary_sw->isr();
	}

	if (!(ticks & TIMER_MASK_819_2_MS)) {
		EventQueue::instance()->post(EVENT_GET_SPDIF_STATUS);
		ticks = 0;
	}
#endif
}

SystemControl* SystemControl::instance()
{
	static SystemControl inst;
	return &inst;
}
