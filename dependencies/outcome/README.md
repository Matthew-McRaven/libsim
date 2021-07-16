# Where did this library come from?
This is a modified version of: https://github.com/ned14/outcome
I had to strip all code which touched coroutines, because clang support of coroutines is very bad.
When clang fully supports coroutines, I will jettison this library and use the Boost version.
I am also using the experimental version of the API, hopping to get fast compile times.

Until then, please please don't try and update outcome.hpp

# Why??
WASM deals poorly with exceptions.
Exceptions thrown inside the C++-compiled-to-WASM cannot be handled within WASM.
That is, catch() is useless.
Previouly in Pep/9, I used exceptions to handle lack of memory-mapped input.
Now that I can't use exceptions, I need a new signalling mechanism.
Enter outcome<T,S,P>, which allows you to differentiate recoverable (wide contract) errors from unrecoverable (narrow contract) errors.
Instead of returning T (and possibly throwing), the return type will now explicitly encode errors.