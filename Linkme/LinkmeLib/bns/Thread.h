#pragma once
#include <functional>
#include <tuple>
#include <type_traits>
#include <Windows.h>
#include <atomic>
#include <stdexcept>
#include <xthreads.h>
#include <thread>
#include <queue>
#include <future>
#include <mutex>
#include <winrt/base.h>

namespace bns
{
	class Thread
	{
	public:

		/// <summary>
		/// ����һ��û�����̵߳�ʵ�����Ժ���Ե���Launch�������߳�
		/// </summary>
		explicit Thread()
		{
			delegateQueueSema = winrt::check_pointer(CreateSemaphore(0, 0, LONG_MAX, 0));
		}

		/// <summary>
		/// ���첢����һ�����̣߳����ò�����std::thread��ͬ��BnThread��������ں����ķ���ֵ
		/// </summary>
		/// <param name="f">���̵߳���ں���</param>
		/// <param name="...args">��ں����Ĳ���</param>
		template<class FUNCTION, class ...ARGS>
		explicit Thread(FUNCTION&& f, ARGS&&...args)
			:Thread()
		{
			Lanuch(std::forward<FUNCTION>(f), std::forward<ARGS>(args)...);
		}

		/// <summary>
		/// ����һ�����̣߳�ʹ�÷�ʽ�빹�캯��BnThread(FUNCTION&& f, ARGS&&...args)��ͬ
		/// </summary>
		/// <param name="f">���̵߳���ں���</param>
		/// <param name="args">��ں����Ĳ���</param>
		template<class FUNCTION, class ...ARGS>
		void Lanuch(FUNCTION&& f, ARGS&&...args)
		{
			using Tuple = std::tuple<std::decay_t<FUNCTION>, std::decay_t<ARGS>...>;
			auto vals = std::make_unique<Tuple>(std::forward<FUNCTION>(f), std::forward<ARGS>(args)...);
			constexpr auto proc = GetInvoke<Tuple>(std::make_index_sequence<1 + sizeof...(ARGS)>{});

			quitEvent = winrt::check_pointer(CreateEvent(0, TRUE, FALSE, NULL));

			std::thread t(proc, this, (void*)(vals.get()));
			t.swap(th);

			vals.release();
		}

		/// <summary>
		/// ����һ�����߳�����ִ���첽���ã�ʹ��BeginInvoke�����첽����
		/// </summary>
		void Lanuch()
		{
			Lanuch(&Thread::ProcessAllDelegate, this);
		}

		/// <summary>
		/// ����߳��Ƿ�����������
		/// </summary>
		/// <returns></returns>
		inline bool IsRuning()
		{
			return th.joinable();
		}

		/// <summary>
		/// �����첽���ã��������ʹ�ô���ڲ�����Lanuch�������̵߳ģ������̺߳����ڲ�����ProcessAccumulateDelegate�������첽���ã�
		/// �������ʹ���޲�����Lanuch�������̵߳ģ���ôBnThread���ᰴ�ն��з�ʽִ���첽����
		/// </summary>
		/// <param name="action"></param>
		void BeginInvoke(std::function<void()> action)
		{
			if (std::this_thread::get_id() == th.get_id())
			{
				action();
				return;
			}
			delegateQueueMutex.lock();
			delegateQueue.push(action);
			delegateQueueMutex.unlock();

			winrt::check_hresult(ReleaseSemaphore(delegateQueueSema, 1, 0));
		}

		/// <summary>
		/// ������ֵ��ͬ�����ã�
		/// ʹ��Invoke�ķ���ֵ�Կ����Ƿ�ʹ��ͬ�����ã�����㲻���Invoke�ķ���ֵ����Invoke�˻�ΪBeginInvoke
		/// </summary>
		/// <typeparam name="TRet">����ֵ����</typeparam>
		/// <param name="func">ͬ�����ú���</param>
		/// <returns></returns>
		template<typename TRet>
		std::shared_ptr<std::promise<TRet>> Invoke(std::function<TRet()> func)
		{
			std::shared_ptr<std::promise<TRet>> resultPromise = std::make_shared<std::promise<TRet>>();
			BeginInvoke([resultPromise, func]()
				{
					try
					{
						resultPromise->set_value(func());
					}
					catch (...)
					{
						resultPromise->set_exception(std::current_exception());
					}
				});

			return resultPromise;
		}

		std::shared_ptr<std::promise<void>> Invoke(std::function<void()> func)
		{
			std::shared_ptr<std::promise<void>> resultPromise = std::make_shared<std::promise<void>>();
			BeginInvoke([resultPromise, func]()
				{
					try
					{
						func();
						resultPromise->set_value();
					}
					catch (...)
					{
						resultPromise->set_exception(std::current_exception());
					}
				});
			
			return resultPromise;
		}

		/// <summary>
		/// ����Ƿ�����߳̽������󣬸���������������̵߳���RequestInterrupt����
		/// �û��̺߳���Ӧ�����ʵ���ʱ�������������ȷ���Ƿ�Ӧ�ý����û��߳�
		/// </summary>
		/// <returns></returns>
		inline bool IsRequestInterrupt() const
		{
			return requestInterrupt;
		}

		/// <summary>
		/// �ȴ��߳̽���
		/// </summary>
		/// <returns>�߳��ڵ���Wait֮ǰ�Ѿ������򷵻�false�����������ȴ��߳�ֱ�������ٷ���true</returns>
		bool Wait() noexcept
		{
			if (th.joinable())
			{
				th.join();
				return true;
			}

			return false;
		}

		/// <summary>
		/// �����ۻ����첽����
		/// �������Ӧ�����û��Զ�����̺߳������ʵ���ʱ������Դ������������������첽��������
		/// ������ִ�����ϴε����������ۻ��������첽���òŷ���
		/// </summary>
		void ProcessAccumulateDelegate()
		{
			std::function<void()> f;
			while (WaitForSingleObject(delegateQueueSema, 0) == WAIT_OBJECT_0)
			{
				delegateQueueMutex.lock();
				f = delegateQueue.back();
				delegateQueue.pop();
				delegateQueueMutex.unlock();
				f();
			}
		}

		size_t AccumulateDelegate()
		{
			delegateQueueMutex.lock();
			auto num = delegateQueue.size();
			delegateQueueMutex.unlock();
			return num;
		}

		/// <summary>
		/// �������е��첽����
		/// ������һֱִ�л�ȴ������̵߳������첽���ã�ֱ��ʹ��RequestInterrupt�������
		/// </summary>
		void ProcessAllDelegate()
		{
			std::function<void()> f;
			while (WaitForSingleObject(delegateQueueSema, INFINITE) == WAIT_OBJECT_0 && !IsRequestInterrupt())
			{
				delegateQueueMutex.lock();
				f = delegateQueue.back();
				delegateQueue.pop();
				delegateQueueMutex.unlock();
				f();
			}
		}

		/// <summary>
		/// �����߳̽������û��߳�Ӧ��ʹ��IsRequestInterrupt��鲢��Ӧ�������
		/// </summary>
		/// <returns></returns>
		inline void RequestInterrupt() noexcept
		{
			requestInterrupt = true;
			BeginInvoke([]() {});
		}

		virtual ~Thread()
		{
			if (th.joinable())
			{
				RequestInterrupt();
				th.join();
			}

			CloseHandle(delegateQueueSema);
		}

		static std::function<void(std::exception_ptr)> exceptHandler;

	private:
		template<class Tuple, size_t...Indices>
		_NODISCARD static constexpr auto GetInvoke(std::index_sequence<Indices...>) noexcept
		{
			return &RunWarpper<Tuple, Indices...>;
		}

		template<class Tuple, size_t...Indices>
		void RunWarpper(void* vals)
		{
			const std::unique_ptr<Tuple> fnVals(static_cast<Tuple*>(vals));
			Tuple& tup = *fnVals;

			try
			{
				std::invoke(std::move(std::get<Indices>(tup))...);
			}
			catch (...)
			{
				_Cnd_do_broadcast_at_thread_exit();
				SetEvent(quitEvent);

				if (exceptHandler)
					exceptHandler(std::current_exception());
				else
					std::terminate();

				return;
			}

			_Cnd_do_broadcast_at_thread_exit();
			SetEvent(quitEvent);
		}

		std::queue<std::function<void()>> delegateQueue;
		std::mutex delegateQueueMutex;
		HANDLE delegateQueueSema = NULL;

		HANDLE quitEvent = NULL;
		std::thread th;
		std::atomic_bool requestInterrupt = false;
	};
}
