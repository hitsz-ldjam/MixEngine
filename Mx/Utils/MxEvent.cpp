#include "MxEvent.h"
#include <cassert>

namespace Mix {

	ConnectionDataBase::~ConnectionDataBase() {
		assert(handleCount == 0 && !isEnable);
	}

	void ConnectionDataBase::disable() {
		isEnable = false;
	}

	EventInternalData::~EventInternalData() {
		auto* connec = mConnections;
		while (connec != nullptr) {
			auto next = connec->next;
			delete connec;
			connec = next;
		}

		connec = mFreeConnections;
		while (connec != nullptr) {
			auto next = connec->next;
			delete connec;
			connec = next;
		}

		connec = mNewConnections;
		while (connec != nullptr) {
			auto next = connec->next;
			delete connec;
			connec = next;
		}
	}

	void EventInternalData::connect(ConnectionDataBase* _cod) {
		_cod->prev = mLastConnection;

		if (mLastConnection != nullptr)
			mLastConnection->next = _cod;

		mLastConnection = _cod;

		if (mConnections == nullptr)
			mConnections = _cod;
	}

	void EventInternalData::disconnect(ConnectionDataBase* _cod) {
		std::unique_lock<std::recursive_mutex> lock(mMutex);

		_cod->disable();
		--_cod->handleCount;

		if (_cod->handleCount == 0)
			free(_cod);
	}

	void EventInternalData::clear() {
		std::unique_lock<std::recursive_mutex> lock(mMutex);

		ConnectionDataBase* connec = mConnections;
		while (connec != nullptr) {
			auto next = connec->next;
			connec->disable();

			if (connec->handleCount == 0)
				free(connec);

			connec = next;
		}

		mConnections = nullptr;
		mLastConnection = nullptr;
	}

	void EventInternalData::freeHandle(ConnectionDataBase* _cod) {
		std::unique_lock<std::recursive_mutex> lock(mMutex);

		--_cod->handleCount;

		if (_cod->handleCount == 0 && !_cod->isEnable)
			free(_cod);
	}

	void EventInternalData::free(ConnectionDataBase* _cod) {
		if (_cod->prev != nullptr)
			_cod->prev->next = _cod->next;
		else
			mConnections = _cod->next;

		if (_cod->next != nullptr)
			_cod->next->prev = _cod->prev;
		else
			mLastConnection = _cod->prev;

		_cod->prev = _cod->next = nullptr;

		if (mFreeConnections != nullptr) {
			_cod->next = mFreeConnections;
			mFreeConnections->prev = _cod;
		}

		mFreeConnections = _cod;
		mFreeConnections->~ConnectionDataBase();
	}

	EventHandle::~EventHandle() {
		if (mConnection != nullptr)
			mEventData->freeHandle(mConnection);
	}

	void EventHandle::disconnect() {
		if (mConnection != nullptr) {
			mEventData->disconnect(mConnection);
			mConnection = nullptr;
			mEventData = nullptr;
		}
	}

	EventHandle& EventHandle::operator=(const EventHandle& _other) {
		mConnection = _other.mConnection;
		mEventData = _other.mEventData;

		if (mConnection != nullptr)
			++mConnection->handleCount;

		return *this;
	}
}
