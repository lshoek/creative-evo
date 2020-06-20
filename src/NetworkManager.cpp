#include "NetworkManager.h"
#include "OscProtocol.h"

void NetworkManager::setup(std::string host, int inPort, int outPort)
{
	_bufferSender.setup(host, outPort);

	_sender.setup(host, outPort);
	_receiver.setup(inPort);

	_state = HANDSHAKE;
}

void NetworkManager::allocate(size_t numJoints, size_t numOutputs, uint32_t w, uint32_t h, ofPixelFormat type)
{
	_outputBufferSize = numOutputs;
	_outputBuffer.resize(numOutputs);

	_jointsInputBufferSize = numJoints;
	_jointsInputBuffer.resize(numJoints);

	_bufferSender.allocate(w, h, type);
}

void NetworkManager::sendState(SimInstance* instance)
{
	// todo: append joint states... (instance->getCreature()->getJointState())
	_bufferSender.send(instance->getCanvas()->getConvPixelBuffer(), instance->getID());
}

void NetworkManager::send(std::string addr)
{
	ofxOscMessage msg;
	msg.setAddress(addr);

	_sender.sendMessage(msg);
}

void NetworkManager::send(std::string addr, std::string arg)
{
	ofxOscMessage msg;
	msg.setAddress(addr);
	msg.addStringArg(arg);

	_sender.sendMessage(msg);
}

void NetworkManager::send(std::string addr, int arg)
{
	ofxOscMessage msg;
	msg.setAddress(addr);
	msg.addIntArg(arg);

	_sender.sendMessage(msg);
}

void NetworkManager::send(std::string addr, double arg)
{
	ofxOscMessage msg;
	msg.setAddress(addr);
	msg.addDoubleArg(arg);

	_sender.sendMessage(msg);
}

void NetworkManager::receive()
{
	if (_receiver.isListening()) {
		bool bMsgInQueue = _receiver.hasWaitingMessages();

		if (bMsgInQueue) {
			ofBuffer buf;
			bool bMsgStart = false;
			int numFrameParts = 0;

			while (_receiver.hasWaitingMessages()) {
				ofxOscMessage m;
				_receiver.getNextMessage(&m);

				std::vector<std::string> tokens = ofSplitString(m.getAddress(), "/");
				const std::string& addr_id = tokens[1];

				if (addr_id == OSC_HELLO_IN) {
					if (_repeatMessageScheduler.isThreadRunning()) {
						_repeatMessageScheduler.stopThread();
						_handshakeListener.unsubscribe();
					}
					_state = ACTIVE;
					onConnectionEstablished.notify();
					break;
				}
				if (addr_id == OSC_INFO_IN) {
					SimInfo info;
					info.id = ofToInt(tokens[2]);
					info.generation = ofToInt(tokens[3]);
					info.duration = ofToInt(tokens[4]);
					onInfoReceived.notify(info);
					break;
				}
				if (addr_id == OSC_BYE_IN) {
					onConnectionClosed.notify();
					break;
				}

				// Receive neural network output vector
				if (addr_id == OSC_ACTIVATION) {
					_queuedAgentId = ofToInt(tokens[2]);
					buf.append(m.getArgAsBlob(0));
					memcpy(&_outputBuffer[0], buf.getData(), _outputBufferSize * sizeof(float));
					_bOutputQueued = true;
				}
				if (addr_id == OSC_ACTIVATION_BUF) {
					//const std::string& addr_info = tokens[2];

					//if (addr_info == OSC_START) {
					//	bMsgStart = true;
					//}
					//else if (addr_info == OSC_PART) {
					//	if (!bMsgStart) continue;
					//	buf.append(m.getArgAsBlob(0));
					//	numFrameParts++;
					//}
					//else if (addr_info == OSC_END) {
					//	if (!bMsgStart) continue;
					//	if (buf.size() > 0) {
					//		break;
					//	}
					//}
				}
			}
			//std::ostringstream ss;
			//ss << "received " << buf.size() << " bytes over " << numFrameParts << " messages.";
			//ofLog() << ss.str();
		}
	}
}

void NetworkManager::search()
{
	if (!_repeatMessageScheduler.isThreadRunning()) {
		_handshakeListener = _repeatMessageScheduler.tick.newListener([this] {
			send(OSC_HELLO, "");
			ofLog() << '.';
		});
		_repeatMessageScheduler.setup(1.0f);
		_repeatMessageScheduler.startThread();
	}
}

void NetworkManager::close()
{
	send(OSC_BYE, "");

	if (_repeatMessageScheduler.isThreadRunning()) {
		_repeatMessageScheduler.stopThread();
		_handshakeListener.unsubscribe();
	}
	_sender.clear();
	_receiver.stop();
}

bool NetworkManager::isAgentOutputQueued()
{
	return _bOutputQueued;
}

uint32_t NetworkManager::getQueuedAgentId()
{
	return _queuedAgentId;
}

const std::vector<float>& NetworkManager::popOutputBuffer()
{
	_bOutputQueued = false;
	return _outputBuffer;
}

BufferSender& NetworkManager::getBufferSender()
{
	return _bufferSender;
}