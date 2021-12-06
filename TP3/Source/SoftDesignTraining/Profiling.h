#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <map>

/// <summary>
/// A namespace providing profiling utilities.
/// </summary>
namespace profiling {

	/// <summary>
	/// A chronometer that uses the system clock to measure elapsed time.
	/// </summary>
	class Chronometer {
	public:
		/// <summary>
		/// Starts the chronometer.
		/// </summary>
		void start() {
			m_startTime = std::chrono::system_clock::now();
			m_endTime = std::chrono::system_clock::now();
		}

		/// <summary>
		/// Stops the chronometer.
		/// </summary>
		void stop() {
			m_endTime = std::chrono::system_clock::now();
			m_totalTime += getElapsedSeconds();
		}

		/// <summary>
		/// Gets the elapsed time (in seconds) from start to stop.
		/// </summary>
		/// <returns>The elapsed time (in seconds) from start to stop.</returns>
		double getElapsedSeconds() {
			std::chrono::duration<double> elapsed_seconds = m_endTime - m_startTime;
			return abs(elapsed_seconds.count()) < 1000 ? elapsed_seconds.count() : 0;
		}

		/// <summary>
		/// Gets the elapsed time (in seconds) from start to stop.
		/// </summary>
		/// <returns>The elapsed time (in seconds) from start to stop.</returns>
		double getTotalTime() {
			return m_totalTime;
		}

		/// <summary>
		/// Resets the chronometer.
		/// </summary>
		void reset() {
			m_startTime = std::chrono::time_point<std::chrono::system_clock>();
			m_endTime = std::chrono::time_point<std::chrono::system_clock>();
			m_totalTime = 0;
		}

	private:
		std::chrono::time_point<std::chrono::system_clock> m_startTime;
		std::chrono::time_point<std::chrono::system_clock> m_endTime;
		double m_totalTime;
	};

	/// <summary>
	/// A profiler that keeps track of elapsed time for various program scopes.
	/// </summary>
	class Profiler {
	public:
		/// <summary>
		/// Creates a profiler instance.
		/// </summary>
		Profiler() {
			for (const auto& scope : scopes) {
				m_scopeChronometers.insert(std::make_pair(scope, Chronometer()));
			}
		}

		/// <summary>
		/// Starts the profiling timer for the given scope.
		/// </summary>
		/// <param name="scope">The profiled scope.</param>
		void startProfilingScope(const std::string& scope) {
			m_scopeChronometers[scope].start();
		}

		/// <summary>
		/// Stops the profiling timer for the given scope.
		/// </summary>
		/// <param name="scope">The profiled scope.</param>
		void stopProfilingScope(const std::string& scope) {
			m_scopeChronometers[scope].stop();
		}

		/// <summary>
		/// Gets the elapsed time (in seconds) from start to stop for the given scope.
		/// </summary>
		/// <param name="scope">The profiled scope.</param>
		/// <returns>The elapsed time (in seconds) from start to stop for the given scope.</returns>
		double getScopeElapsedSeconds(const std::string& scope) {
			return m_scopeChronometers[scope].getElapsedSeconds();
		}

		/// <summary>
		/// Resets the profiling timers for all scopes.
		/// </summary>
		void reset() {
			for (auto &scopeChronometer : m_scopeChronometers) {
				scopeChronometer.second.reset();
			}
		}

		/// <summary>
		/// The scopes to profile.
		/// </summary>
		std::vector<std::string> scopes = {
				"UPDATE",
				"DETECT",
				"FLEE",
				"COLLECT"
		};

	private:
		/// <summary>
		/// The chronometers associated with the scopes to profile.
		/// </summary>
		std::map<std::string, Chronometer> m_scopeChronometers;
	};

}
