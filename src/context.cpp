#include "context.hpp"

#include <iostream>
#include <algorithm>

namespace audio {

	void Context::addNode(Node* node) {
		nodes.push_back(node);
	}

	void Context::rmvNode(Node* node) {
		auto iter = std::find(nodes.begin(), nodes.end(), node);
		if (iter != nodes.end())
			nodes.erase(iter);
	}

	void Context::addDevice(Device* device) {
		devices.push_back(device);
	}

	void Context::rmvDevice(Device* device) {
		auto iter = std::find(devices.begin(), devices.end(), device);
		if (iter != devices.end())
			devices.erase(iter);
	}

	Context::Context() {
		int err = Pa_Initialize();
		if (err < 0) {
			std::cerr << "Error, couldn't initialize PortAudio: " << Pa_GetErrorText(err) << std::endl;
			return;
		}

		SineTable::init();
		SquareTable::init();
		SawTable::init();

		initialized = true;
	}
	Context::~Context() {
		if (initialized)
			Pa_Terminate();
	}

	DeviceInfo Context::getDeviceInfo(int index) const {
		if (!initialized) {
			std::cerr << "Error, context is not initialized" << std::endl;
			return DeviceInfo();
		}

		const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
		DeviceInfo result;

		// Commmon
		result.index = index;
		result.name = deviceInfo->name;
		result.defaultSampleRate = deviceInfo->defaultSampleRate;
		result.hostApiIndex = deviceInfo->hostApi;

		// Input
		result.input.maxChannels = deviceInfo->maxInputChannels;
		result.input.defaultLowLatency = deviceInfo->defaultLowInputLatency;
		result.input.defaultHighLatency = deviceInfo->defaultHighInputLatency;

		// Output
		result.output.maxChannels = deviceInfo->maxOutputChannels;
		result.output.defaultLowLatency = deviceInfo->defaultLowOutputLatency;
		result.output.defaultHighLatency = deviceInfo->defaultHighOutputLatency;
		return result;
	}

	HostApi Context::getHostApi(int index) const {
		const PaHostApiInfo* hostApiInfo = Pa_GetHostApiInfo(index);
		HostApi hostApi;
		hostApi.index = hostApiInfo->type;
		hostApi.name = hostApiInfo->name;
		hostApi.deviceCount = hostApiInfo->deviceCount;
		hostApi.defaultInputDeviceIndex = hostApiInfo->defaultInputDevice;
		hostApi.defaultOutputDeviceIndex = hostApiInfo->defaultOutputDevice;
		return hostApi;
	}

	void Context::update(double sampleRate) {
		for (auto& node: nodes)
			node->update(sampleRate);
	}

}