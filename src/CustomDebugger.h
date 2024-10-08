#pragma once
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#include <mallocator.h>
#include <list>

struct Allocation;

namespace MYCRT {
	struct MemoryDebuggerInitializer
	{
		MemoryDebuggerInitializer();
		~MemoryDebuggerInitializer();
		static int s_count;
	};
	static MemoryDebuggerInitializer s_logInit;

	enum class OperatorType {
		NONE, NEW, NEW_ARRAY, DEL, DEL_ARRAY
	};
	static bool AreTypesMatching(MYCRT::OperatorType allocType, MYCRT::OperatorType deallocType);

	template<typename T>
	using list = std::list<T, Mallocator<T>>;

	struct AllocInfo {
		void* basePtr;
		void* ptr;
		DWORD64 rip;
		size_t size;
		MYCRT::OperatorType opType;
	public:
		AllocInfo() : basePtr{}, ptr{}, rip{}, size{}, opType{ MYCRT::OperatorType::NONE } {}
	};


	class MemoryDebugger {
		static constexpr int NUM_INTERNAL_CALLS = 2;//2 internal calls
		static constexpr int PAGE_SIZE = 4096;

		MYCRT::list<AllocInfo> allocated_list; // private member of debugger class
	private:
		void InitSym();
		void SymbolsInit();
		void FillStackFrame(STACKFRAME64& stack_frame, const CONTEXT& context);

		bool Deallocate(void* basePtr);	// Set the size parameter to 0 to resolve warnings generated by a non-zero value with the MEM_DECOMMIT flag.
		bool Release(void* address);// Set the size parameter to 0 per Microsoft's documentation when using the MEM_RELEASE flag.

		DWORD64 StackTrace(const CONTEXT* start_context);
		IMAGEHLP_LINE64 GetSymbols(DWORD64 returnAddress);

	public:
		static MemoryDebugger* s_instance;

		void* PageAlignedAllocate(size_t size);
		void ObserveMemory(size_t size, OperatorType);
		bool MemoryDeallocated(DWORD64& memory, OperatorType deleteType);

		MemoryDebugger() = default;
		MemoryDebugger(const MemoryDebugger&) = delete;
		MemoryDebugger(MemoryDebugger&&) noexcept = delete;
		MemoryDebugger& operator=(const MemoryDebugger&) = delete;
		MemoryDebugger& operator=(MemoryDebugger&&) noexcept = delete;
		~MemoryDebugger();
	};
}

//global new(s) & delete(s)
void* operator new(size_t size);
void* operator new(size_t size, const std::nothrow_t) noexcept;
void* operator new[](size_t size);
void* operator new[](size_t size, const std::nothrow_t&) noexcept;
void operator delete(void* address) noexcept;
void operator delete(void* address, size_t size) noexcept;
void operator delete(void* address, const std::nothrow_t) noexcept;
void operator delete[](void* address) noexcept;
void operator delete[](void* address, size_t size) noexcept;
void operator delete[](void* address, const std::nothrow_t) noexcept;
