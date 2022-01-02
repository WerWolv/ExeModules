#pragma once

#include <utility>

namespace cht::hlp {

    #define TOKEN_CONCAT_IMPL(x, y) x ## y
    #define TOKEN_CONCAT(x, y) TOKEN_CONCAT_IMPL(x, y)
    #define ANONYMOUS_VARIABLE(prefix) TOKEN_CONCAT(prefix, __COUNTER__)

    namespace scope_guard {

        #define SCOPE_GUARD ::cht::hlp::scope_guard::ScopeGuardOnExit() + [&]()
        #define ON_SCOPE_EXIT auto ANONYMOUS_VARIABLE(SCOPE_EXIT_) = SCOPE_GUARD

        template<class F>
        class ScopeGuard {
        private:
            F m_func;
            bool m_active;
        public:
            constexpr ScopeGuard(F func) : m_func(std::move(func)), m_active(true) { }
            ~ScopeGuard() { if (this->m_active) { this->m_func(); } }
            void release() { this->m_active = false; }

            ScopeGuard(ScopeGuard &&other) noexcept : m_func(std::move(other.m_func)), m_active(other.m_active) {
                other.cancel();
            }

            ScopeGuard& operator=(ScopeGuard &&) = delete;
        };

        enum class ScopeGuardOnExit { };

        template <typename F>
        constexpr ScopeGuard<F> operator+(ScopeGuardOnExit, F&& f) {
            return ScopeGuard<F>(std::forward<F>(f));
        }

    }

    namespace first_time_exec {

        #define FIRST_TIME static auto ANONYMOUS_VARIABLE(FIRST_TIME_) = ::cht::hlp::first_time_exec::FirstTimeExecutor() + [&]()

        template<class F>
        class FirstTimeExecute {
        public:
            constexpr FirstTimeExecute(F func) { func(); }

            FirstTimeExecute& operator=(FirstTimeExecute &&) = delete;
        };

        enum class FirstTimeExecutor { };

        template <typename F>
        constexpr FirstTimeExecute<F> operator+(FirstTimeExecutor, F&& f) {
            return FirstTimeExecute<F>(std::forward<F>(f));
        }

    }

    namespace final_cleanup {

        #define FINAL_CLEANUP static auto ANONYMOUS_VARIABLE(ON_EXIT_) = ::cht::hlp::final_cleanup::FinalCleanupExecutor() + [&]()

        template<class F>
        class FinalCleanupExecute {
            F m_func;
        public:
            constexpr FinalCleanupExecute(F func) : m_func(func) { }
            constexpr ~FinalCleanupExecute() { this->m_func(); }

            FinalCleanupExecute& operator=(FinalCleanupExecute &&) = delete;
        };

        enum class FinalCleanupExecutor { };

        template <typename F>
        constexpr FinalCleanupExecute<F> operator+(FinalCleanupExecutor, F&& f) {
            return FinalCleanupExecute<F>(std::forward<F>(f));
        }

    }

}