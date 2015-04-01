#pragma once

#include <pthread.h>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string.h>

/* <<< Using threads >>>
 * class MyThread : public Thread (or JoinThread)
 * {
 *   void execute()
 *   {
 *     // write the thread's code here
 *   }
 * };
 *
 * MyThread myThread;
 *
 * // Calls from outside the thread
 * myThread.run(); // start the thread
 * myThread.cancel(); // stop the thread (returns true if it really was running, false otherwise)
 * myThread.join(); // joins the thread (method available only in JoinThread)
 *
 * // Calls from inside the thread
 * myThread.cancelDisable(); // turn off canceling
 * myThread.cacnelEnable(); // turn on canceling
 *
 * <<< Using mutexes >>>
 * Mutex mutex;
 * mutex.lock();
 * mutex.unlock();
 *
 * <<< Using condition variables >>>
 * Condition cond(mutex);
 * // Methods below assume that the mutex (given in the constructor) is locked at the moment of call.
 * cond.wait();
 * cond.signal();
 */

namespace yolo
{
	class PthreadException : public std::exception
	{
	public:
		PthreadException(int errorCode) : message(strerror(errorCode)) {}
		PthreadException(const std::string & message) : message(message) {}
		const char * what() const noexcept override
		{
			return message.c_str();
		}
	
	private:
		const std::string message;
	};

	template<typename DerivedClass>
	class ThreadBase
	{
		friend class Thread;
		friend class JoinThread;

	public:
		class ThreadException : public PthreadException
		{
			using PthreadException::PthreadException;
		};

		ThreadBase(int flag) : isRunning(false), wasCanceled(false), attr(std::make_shared<pthread_attr_t>())
		{
			int err;
			if(err = pthread_attr_init(&(*attr)))
				throw ThreadException(err);
			if(err = pthread_attr_setdetachstate(&(*attr), flag))
				throw ThreadException(err);
		}

		~ThreadBase()
		{
			if(attr.unique())
				pthread_attr_destroy(&(*attr));
		}

		void run(DerivedClass * who)
		{
			if(isRunning)
				throw ThreadException("run(): Thread has already started.");
			isRunning = true;
			int err;
			if(err = pthread_create(&thread, &(*attr), grossExecute, (void *) who))
				throw ThreadException(err);
		}

		void join()
		{
			if(!isRunning)
				throw ThreadException("join(): Thread is not running.");
			int err;
			if(err = pthread_join(thread, 0))
				throw ThreadException(err);
			isRunning = false;
		}

		bool cancel()
		{
			if(!isRunning)
				throw ThreadException("cancel(): Thread is not running.");
			if(wasCanceled)
				throw ThreadException("cancel(): Thread was already canceled.");
			int err;
			if(err = pthread_cancel(thread))
			{
				if(err == ESRCH)
					return false;
				throw ThreadException(err);
			}
			return true;
		}

		void cancelDisable()
		{
			int err;
			if(err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0))
				throw ThreadException(err);
		}

		void cancelEnable()
		{
			int err;
			if(err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0))
				throw ThreadException(err);
		}
	
	private:
		bool isRunning;
		bool wasCanceled;
		std::shared_ptr<pthread_attr_t> attr;
		pthread_t thread;

		static void * grossExecute(void * data)
		{
			((DerivedClass *) data)->execute();
			return 0;
		}
	};

	class Thread : private ThreadBase<Thread>
	{
		friend ThreadBase<Thread>;

	public:
		Thread() : ThreadBase<Thread>(PTHREAD_CREATE_DETACHED) { }
		virtual ~Thread() { }

		void run() { ThreadBase<Thread>::run(this); }
		bool cancel() { return ThreadBase<Thread>::cancel(); }
	
	protected:
		void cancelEnable() { ThreadBase<Thread>::cancelEnable(); }
		void cancelDisable() { ThreadBase<Thread>::cancelDisable(); }

		virtual void execute() = 0;
	};

	class JoinThread : private ThreadBase<JoinThread>
	{
		friend ThreadBase<JoinThread>;
	
	public:
		JoinThread() : ThreadBase<JoinThread>(PTHREAD_CREATE_JOINABLE) { }
		virtual ~JoinThread()
		{
			if(isRunning)
				join();
		}

		void run() { ThreadBase<JoinThread>::run(this); }
		void join() { ThreadBase<JoinThread>::join(); }
		void cancel() { ThreadBase<JoinThread>::cancel(); }
	
	protected:
		void cancelEnable() { ThreadBase<JoinThread>::cancelEnable(); }
		void cancelDisable() { ThreadBase<JoinThread>::cancelDisable(); }

		virtual void execute() = 0;
	};

	class Mutex
	{
		friend class Condition;

	public:
		class MutexException : public PthreadException
		{
			using PthreadException::PthreadException;
		};

		Mutex() : mutex(std::make_shared<pthread_mutex_t>())
		{
			int err;
			if(err = pthread_mutex_init(&(*mutex), 0))
				throw MutexException(err);
		}

		~Mutex()
		{
			if(mutex.unique())
				pthread_mutex_destroy(&(*mutex));
		}

		void lock()
		{
			int err;
			if(err = pthread_mutex_lock(&(*mutex)))
				throw MutexException(err);
		}

		void unlock()
		{
			int err;
			if(err = pthread_mutex_unlock(&(*mutex)))
				throw MutexException(err);
		}
		
	private:
		std::shared_ptr<pthread_mutex_t> mutex;
	};

	class Condition
	{
	public:
		class ConditionException : public PthreadException
		{
			using PthreadException::PthreadException;
		};

		Condition(const Mutex & mutex) : mutex(mutex), condition(std::make_shared<pthread_cond_t>())
		{
			int err;
			if(err = pthread_cond_init(&(*condition), 0))
				throw ConditionException(err);
		}

		~Condition()
		{
			if(condition.unique())
				pthread_cond_destroy(&(*condition));
		}

		void signal()
		{
			int err;
			if(err = pthread_cond_signal(&(*condition)))
				throw ConditionException(err);
		}

		void wait()
		{
			int err;
			if(err = pthread_cond_wait(&(*condition), &(*mutex.mutex)))
				throw ConditionException(err);
		}

	private:
		Mutex mutex;
		std::shared_ptr<pthread_cond_t> condition;
	};

} // namespace yolo
