#pragma once
#ifndef MX_EVENT_H_
#define MX_EVENT_H_

#include <mutex>
#include <functional>

namespace Mix {
	class ConnectionDataBase {
	public:
		ConnectionDataBase() = default;

		virtual ~ConnectionDataBase();

		virtual void disable();

		ConnectionDataBase* prev = nullptr;
		ConnectionDataBase* next = nullptr;
		bool isEnable = true;
		uint32_t handleCount = 0;
	};

	class EventInternalData {
	public:
		EventInternalData() = default;

		~EventInternalData();

		void connect(ConnectionDataBase* _cod);

		void disconnect(ConnectionDataBase* _cod);

		void clear();

		void freeHandle(ConnectionDataBase* _cod);

		void free(ConnectionDataBase* _cod);

		ConnectionDataBase* mConnections = nullptr;
		ConnectionDataBase* mLastConnection = nullptr;
		ConnectionDataBase* mFreeConnections = nullptr;
		ConnectionDataBase* mNewConnections = nullptr;

		std::recursive_mutex mMutex;
		bool mIsCurrentlyTriggering = false;
	};

	class EventHandle {
	public:
		EventHandle() = default;

		explicit EventHandle(std::shared_ptr<EventInternalData> _eventData, ConnectionDataBase* _cod)
			:mConnection(_cod), mEventData(std::move(_eventData)) {
			++_cod->handleCount;
		}

		~EventHandle();

		void disconnect();

		EventHandle& operator=(const EventHandle& _other);

	private:
		ConnectionDataBase* mConnection = nullptr;
		std::shared_ptr<EventInternalData> mEventData;
	};


	template<typename _ReType, typename... _Args>
	class EventImpl {
		struct ConnectionData :ConnectionDataBase {
		public:
			void disable() override;

			std::function<_ReType(_Args...)> func;
		};

	public:
		EventImpl() :mInternalData(std::make_shared<EventInternalData>()) {}

		~EventImpl() { clear(); }

		EventHandle connect(std::function<_ReType(_Args...)> _func);

		void operator()(_Args... _args);

		void trigger(_Args... _args);

		void clear() { mInternalData->clear(); }

		bool empty() const;

	private:
		std::shared_ptr<EventInternalData> mInternalData;
	};

	template <typename _ReType, typename ... _Args>
	void EventImpl<_ReType, _Args...>::ConnectionData::disable() {
		func = nullptr;

		ConnectionDataBase::disable();
	}

	template <typename _ReType, typename ... _Args>
	EventHandle EventImpl<_ReType, _Args...>::connect(std::function<_ReType(_Args...)> _func) {
		std::unique_lock<std::recursive_mutex> lock(mInternalData->mMutex);

		ConnectionData* connec = nullptr;

		if (mInternalData->mFreeConnections != nullptr) {
			connec = static_cast<ConnectionData*>(mInternalData->mFreeConnections);
			mInternalData->mFreeConnections = connec->next;
			if (mInternalData->mFreeConnections != nullptr)
				mInternalData->mFreeConnections->prev = nullptr;

			new (connec) ConnectionData();

			connec->isEnable = true;
		}

		if (connec == nullptr)
			connec = new ConnectionData();

		if (mInternalData->mIsCurrentlyTriggering) {
			connec->prev = mInternalData->mNewConnections;

			if (mInternalData->mNewConnections != nullptr)
				mInternalData->mNewConnections->next = connec;

			mInternalData->mNewConnections = connec;
		}
		else {
			mInternalData->connect(connec);
		}

		connec->func = _func;

		return EventHandle(mInternalData, connec);
	}

	template <typename _ReType, typename ... _Args>
	void EventImpl<_ReType, _Args...>::operator()(_Args... _args) {
		trigger(std::forward<_Args>(_args)...);
	}

	template <typename _ReType, typename ... _Args>
	void EventImpl<_ReType, _Args...>::trigger(_Args... _args) {
		// To keep internal data from being destroyed in case one of callbacks destroy Event itself
		auto internalDataRef = mInternalData;

		std::unique_lock<std::recursive_mutex> lock(internalDataRef->mMutex);

		// Begin triggering
		internalDataRef->mIsCurrentlyTriggering = true;

		// Call callbacks one by one
		ConnectionData* connec = static_cast<ConnectionData*>(internalDataRef->mConnections);
		while (connec != nullptr) {
			// To keep next in cast the callback itselt disconnects from this event
			auto next = static_cast<ConnectionData*>(connec->next);

			if (connec->func != nullptr)
				connec->func(std::forward<_Args>(_args)...);

			connec = next;
		}

		// End triggering
		internalDataRef->mIsCurrentlyTriggering = false;

		// If any new connections were added during triggering, add them to connection list
		if (internalDataRef->mNewConnections != nullptr) {
			ConnectionDataBase* lastNewConnec = mInternalData->mNewConnections;
			while (lastNewConnec != nullptr) //Points to the last joined connection
				lastNewConnec = lastNewConnec->next;

			ConnectionDataBase* currConnec = lastNewConnec;
			while (currConnec != nullptr) {
				ConnectionDataBase* prev = currConnec->prev;
				currConnec->next = nullptr;
				currConnec->prev = nullptr;

				internalDataRef->connect(currConnec);
				currConnec = prev;
			}

			internalDataRef->mNewConnections = nullptr;
		}
	}

	template <typename _ReType, typename ... _Args>
	bool EventImpl<_ReType, _Args...>::empty() const {
		std::unique_lock<std::recursive_mutex> lock(mInternalData->mMutex);

		return mInternalData->mConnections == nullptr;
	}


	template<typename _Ty>
	class Event;

	template<typename _ReType, typename... _Args>
	class Event<_ReType(_Args...)> :public EventImpl<_ReType, _Args...> {};
}

#endif
