#define MSC_CLASS "MediaStreamTrackFactory"

#include <iostream>

#include "MediaSoupClientErrors.hpp"
#include "MediaStreamTrackFactory.hpp"
#include "pc/test/fake_audio_capture_module.h"
#include "pc/test/fake_periodic_video_track_source.h"
#include "pc/test/frame_generator_capturer_video_track_source.h"
#include "system_wrappers/include/clock.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "modules/audio_mixer/audio_mixer_impl.h"
#include "test_audio_device.h"

using namespace mediasoupclient;

static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;

/* MediaStreamTrack holds reference to the threads of the PeerConnectionFactory.
 * Use plain pointers in order to avoid threads being destructed before tracks.
 */
static rtc::Thread* networkThread;
static rtc::Thread* signalingThread;
static rtc::Thread* workerThread;

static void createFactory()
{
	networkThread   = rtc::Thread::Create().release();
	signalingThread = rtc::Thread::Create().release();
	workerThread    = rtc::Thread::Create().release();

	networkThread->SetName("network_thread", nullptr);
	signalingThread->SetName("signaling_thread", nullptr);
	workerThread->SetName("worker_thread", nullptr);

	if (!networkThread->Start() || !signalingThread->Start() || !workerThread->Start())
	{
		MSC_THROW_INVALID_STATE_ERROR("thread start errored");
	}

	webrtc::PeerConnectionInterface::RTCConfiguration config;

	auto defaultQueueFActory=webrtc::CreateDefaultTaskQueueFactory();

	auto fakeAudioCaptureModule = webrtc::TestAudioDeviceModule::Create(defaultQueueFActory.get(),
																																			webrtc::TestAudioDeviceModule::CreatePulsedNoiseCapturer(10000, 48000),
																																			webrtc::TestAudioDeviceModule::CreateDiscardRenderer(48000), 1.f);
	if (!fakeAudioCaptureModule)
	{
		MSC_THROW_INVALID_STATE_ERROR("audio capture module creation errored");
	}
	else
	{
		//webrtc::AudioState::Config audio_state_config;
		//audio_state_config.audio_mixer = ;
		//audio_state_config.audio_processing = ;
		//audio_state_config.audio_device_module = fakeAudioCaptureModule;
//		send_call_config->audio_state = webrtc::AudioState::Create(audio_state_config);
//		recv_call_config->audio_state = webrtc::AudioState::Create(audio_state_config);
		//fakeAudioCaptureModule->Init();
//		RTC_CHECK(fakeAudioCaptureModule->RegisterAudioCallback(
//		            send_call_config->audio_state->audio_transport()) == 0);
	}

	factory = webrtc::CreatePeerConnectionFactory(
	  networkThread,
	  workerThread,
	  signalingThread,
	  fakeAudioCaptureModule,
	  webrtc::CreateBuiltinAudioEncoderFactory(),
	  webrtc::CreateBuiltinAudioDecoderFactory(),
	  webrtc::CreateBuiltinVideoEncoderFactory(),
	  webrtc::CreateBuiltinVideoDecoderFactory(),
		nullptr,nullptr//webrtc::AudioMixerImpl::Create() /*audio_mixer*/,
		/*webrtc::AudioProcessingBuilder().Create()*/ /*audio_processing*/);

	if (!factory)
	{
		MSC_THROW_ERROR("error ocurred creating peerconnection factory");
	}

	//fakeAudioCaptureModule->Init();
//	auto numDevice=fakeAudioCaptureModule->RecordingDevices();
//	fakeAudioCaptureModule->InitPlayout();
//	fakeAudioCaptureModule->InitRecording();
//	fakeAudioCaptureModule->InitSpeaker();
//	fakeAudioCaptureModule->SetMicrophoneVolume(100);
//	fakeAudioCaptureModule->SetSpeakerVolume(100);
//	fakeAudioCaptureModule->SetSpeakerMute(false);
//	fakeAudioCaptureModule->StartPlayout();
//	fakeAudioCaptureModule->StartRecording();
}

// Audio track creation.
rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label)
{
	if (!factory)
		createFactory();

	cricket::AudioOptions options;
	options.highpass_filter = false;

	rtc::scoped_refptr<webrtc::AudioSourceInterface> source = factory->CreateAudioSource(options);

	return factory->CreateAudioTrack(label, source);
}

// Video track creation.
rtc::scoped_refptr<webrtc::VideoTrackInterface> createVideoTrack(const std::string& /*label*/)
{
	if (!factory)
		createFactory();

	auto* videoTrackSource =
	  new rtc::RefCountedObject<webrtc::FakePeriodicVideoTrackSource>(false /* remote */);

	return factory->CreateVideoTrack(rtc::CreateRandomUuid(), videoTrackSource);
}

rtc::scoped_refptr<webrtc::VideoTrackInterface> createSquaresVideoTrack(const std::string& /*label*/)
{
	if (!factory)
		createFactory();

	std::cout << "[INFO] getting frame generator" << std::endl;
	auto* videoTrackSource = new rtc::RefCountedObject<webrtc::FrameGeneratorCapturerVideoTrackSource>(
	  webrtc::FrameGeneratorCapturerVideoTrackSource::Config(), webrtc::Clock::GetRealTimeClock(), false);
	videoTrackSource->Start();

	std::cout << "[INFO] creating video track" << std::endl;
	return factory->CreateVideoTrack(rtc::CreateRandomUuid(), videoTrackSource);
}
