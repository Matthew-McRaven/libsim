#include "outcome.hpp"

#define OUTCOME_PREVENT_CONVERSION_WORKAROUND std
template <class T, class S = SYSTEM_ERROR2_NAMESPACE::system_code, class P = std::exception_ptr>
	using outcome = OUTCOME_V2_NAMESPACE::experimental::status_outcome<T, S, P>;