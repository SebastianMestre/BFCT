#pragma once

#include <cstdio>

template<typename... Ts>
inline void log_warn (char const* format, Ts... ts) {
	fprintf(stderr, "\x1b[33mWARN\x1b[0m: ");
	fprintf(stderr, format, ts...);
}

template<typename... Ts>
inline void log_error (char const* format, Ts... ts) {
	fprintf(stderr, "\x1b[31mERROR\x1b[0m: ");
	fprintf(stderr, format, ts...);
}

template<typename... Ts>
inline void log_error_fatal (char const* format, Ts... ts) {
	fprintf(stderr, "\x1b[31mFATAL ERROR\x1b[0m: ");
	fprintf(stderr, format, ts...);
	exit(1);
}

template<typename... Ts>
inline void log_info (char const* format, Ts... ts) {
	fprintf(stderr, "\x1b[34mINFO\x1b[0m: ");
	fprintf(stderr, format, ts...);
}
