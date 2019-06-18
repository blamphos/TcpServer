#include "SpdifSwitch.h"
#include "EventQueue.h"
#include "Parameters.h"

SpdifSwitch::SpdifSwitch(PinName inp1, PinName inp2, PinName inp3) :
		_switch_ctrl(new BusOut(inp1, inp2, inp3))
{
	_parameters = Parameters::instance();
}

void SpdifSwitch::mute()
{
	_switch_ctrl->write(0);
}

void SpdifSwitch::unmute()
{
	Spdif::InputTypeT current_input = _parameters->current_input;

	// This will do the actual switch
	if (current_input != Spdif::Auto) {
		_switch_ctrl->write(1 << current_input);
	}
}

void SpdifSwitch::updateMuteState()
{
	if (_parameters->isMuted()) {
		mute();
	} else {
		unmute();
	}
}

void SpdifSwitch::switchInput(uint32_t data)
{
	Spdif::InputTypeT input = static_cast<Spdif::InputTypeT>(data);

	_parameters->auto_find = (input == Spdif::Auto);
	setActiveInput(input);
}

void SpdifSwitch::updateSpdifStatus(uint32_t data)
{
	static Spdif::SampleRateT input_status[Spdif::INPUT_COUNT] =
			{ Spdif::SR_UNLOCK, Spdif::SR_UNLOCK, Spdif::SR_UNLOCK };

	_parameters->setSpdifStatus(data);

	SpdifStatus::spdif_message_t msg = SpdifStatus::dispatch(data);
	input_status[msg.input] = msg.sample_rate;

	if (_parameters->auto_find) {
		Spdif::InputTypeT input = _parameters->current_input;

		// Select active input according to priority order
		if (input_status[Spdif::Coax1] != Spdif::SR_UNLOCK) {
			input = Spdif::Coax1;
		} else if (input_status[Spdif::Opt1] != Spdif::SR_UNLOCK) {
			input = Spdif::Opt1;
		} else if (input_status[Spdif::Coax2] != Spdif::SR_UNLOCK) {
			input = Spdif::Coax2;
		}

		if (input != _parameters->current_input) {
			setActiveInput(input);
		}
	}
}

void SpdifSwitch::setActiveInput(Spdif::InputTypeT input)
{
	_parameters->current_input = input;
	EventQueue::instance()->post(EVENT_INPUT_CHANGED);
}
