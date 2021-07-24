#pragma once
#include "outcome.hpp"

/*template <class T, class S = SYSTEM_ERROR2_NAMESPACE::system_code, class P = std::exception_ptr>
	using outcome = OUTCOME_V2_NAMESPACE::experimental::status_outcome<T, S, P>;*/
template <class T, class S = SYSTEM_ERROR2_NAMESPACE::system_code>
	using result = OUTCOME_V2_NAMESPACE::experimental::status_result<T, S>;