#ifndef CALLBACK_HANDLER_H
#define CALLBACK_HANDLER_H

/* This class was adapted from the XSens example_mti_receive_data.cpp file */

#include <xscontroller/xscontrol_def.h>
#include <xscontroller/xsdevice_def.h>
#include <xscontroller/xsscanner.h>
#include <xstypes/xsoutputconfigurationarray.h>
#include <xstypes/xsdatapacket.h>
#include <xstypes/xstime.h>
#include <xscommon/xsens_mutex.h>

#include <list>
#include "shared_memory.h"

using namespace std;

typedef void (*parserFunc)(const XsDataPacket *p);

class CallbackHandler : public XsCallback {
	public:
		CallbackHandler(parserFunc f) : parser(f) {}
		/*
			: m_maxNumberOfPacketsInBuffer(maxBufferSize),
			  m_numberOfPacketsInBuffer(0)
			  { }
		
		*/
		virtual ~CallbackHandler() throw() { };

		/*
		bool packetAvailable() const {
			xsens::Lock locky(&m_mutex);
			return m_numberOfPacketsInBuffer > 0;
		}

		XsDataPacket getNextPacket() {
			assert(packetAvailable());
			xsens::Lock locky(&m_mutex);
			XsDataPacket oldestPacket(m_packetBuffer.front());
			m_packetBuffer.pop_front();
			--m_numberOfPacketsInBuffer;
			return oldestPacket;
		}
		*/

	protected:
		void onLiveDataAvailable(XsDevice*, const XsDataPacket* packet) override {
			xsens::Lock locky(&m_mutex);
			assert(packet != 0);
			parser(packet);

		}

	private:
		mutable xsens::Mutex m_mutex;
		//void *shared_mem;
		parserFunc parser;

};

#endif
