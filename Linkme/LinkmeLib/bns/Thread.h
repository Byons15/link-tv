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
		/// 构造一个没有新线程的实例，稍后可以调用Launch启动新线程
		/// </summary>
		explicit Thread()
		{
			delegateQueueSema = winrt::check_pointer(CreateSemaphore(0, 0, LONG_MAX, 0));
		}

		/// <summary>
		/// 构造并启动一个新线程，调用参数与std::thread相同，BnThread将忽略入口函数的返回值
		/// </summary>
		/// <param name="f">新线程的入口函数</param>
		/// <param name="...args">入口函数的参数</param>
		template<class FUNCTION, class ...ARGS>
		explicit Thread(FUNCTION&& f, ARGS&&...args)
			:Thread()
		{
			Lanuch(std::forward<FUNCTION>(f), std::forward<ARGS>(args)...);
		}

		/// <summary>
		/// 启动一个新线程，使用方式与构造函数BnThread(FUNCTION&& f, ARGS&&...args)相同
		/// </summary>
		/// <param name="f">新线程的入口函数</param>
		/// <param name="args">入口函数的参数</param>
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
		/// 启动一个新线程用于执行异步调用，使用BeginInvoke请求异步调用
		/// </summary>
		void Lanuch()
		{
			Lanuch(&Thread::ProcessAllDelegate, this);
		}

		/// <summary>
		/// 检查线程是否正在运行中
		/// </summary>
		/// <returns></returns>
		inline bool IsRuning()
		{
			return th.joinable();
		}

		/// <summary>
		/// 请求异步调用，如果您是使用带入口参数的Lanuch来启动线程的，请在线程函数内部调用ProcessAccumulateDelegate来处理异步调用，
		/// 如果您是使用无参数的Lanuch来启动线程的，那么BnThread将会按照队列方式执行异步调用
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
		/// 带返回值的同步调用，
		/// 使用Invoke的返回值以控制是否使用同步调用，如果你不理会Invoke的返回值，则Invoke退化为BeginInvoke
		/// </summary>
		/// <typeparam name="TRet">返回值类型</typeparam>
		/// <param name="func">同步调用函数</param>
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
		/// 检查是否存在线程结束请求，该请求可以由其他线程调用RequestInterrupt发起
		/// 用户线程函数应该在适当的时候检查这个请求，以确定是否应该结束用户线程
		/// </summary>
		/// <returns></returns>
		inline bool IsRequestInterrupt() const
		{
			return requestInterrupt;
		}

		/// <summary>
		/// 等待线程结束
		/// </summary>
		/// <returns>线程在调用Wait之前已经结束则返回false，否则函数将等待线程直到结束再返回true</returns>
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
		/// 处理累积的异步调用
		/// 这个函数应该由用户自定义的线程函数在适当的时候调用以处理其他函数发来的异步调用请求
		/// 函数将执行自上次调用以来所累积的所有异步调用才返回
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
		/// 处理所有的异步调用
		/// 函数将一直执行或等待发到线程的所有异步调用，直到使用RequestInterrupt请求结束
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
		/// 请求线程结束，用户线程应该使用IsRequestInterrupt检查并响应这个请求
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
